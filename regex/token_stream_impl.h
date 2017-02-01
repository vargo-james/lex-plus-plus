/*
 * Inspect expression_getters for possible errors.
 */

#ifndef _token_stream_impl_h_
#define _token_stream_impl_h_

#include "error_tracker.h"
#include "syntax_option.h"
#include "regex_range.h"
#include "expression_context.h"
#include "regex_token.h"
#include "token_table.h"

#include <locale>
#include <regex>

namespace lex {

template <typename InputIt, typename Traits>
class token_stream_impl : public error_tracker, public syntax_option {
 public:
  using syntax_option::flag_type;
  using error_tracker::error_type;
  using value_type = typename regex_range<InputIt>::value_type;
  using token = regex_token<value_type,Traits>;
  using traits_type = Traits;

  token_stream_impl(regex_range<InputIt>& src, flag_type f, error_type& er)
    : error_tracker(er),
      syntax_option(f),
      table {make_token_table<value_type>(f)},
      source(src),
      con(er, f) {}

  bool empty() const {return source.empty();}
  bool get(token& out);

  expression_context get_context() const {return con;}
 private:
  token_table<value_type> table;
  traits_type traits_;
  regex_range<InputIt>& source;
  expression_context con;

  bool get(token& out, expression_context::context context);
  bool expression_get(token& out);
  bool expression_get_escaped(token& out);
  bool replication_get(token& out);
  bool subexpr_get(token& out);
  bool bracket_get(token& out);
  bool bracket_get_sub_bracket(token& out);
  bool collation_get(token& out, value_type closing_char, 
      token_type closing_token);

  void update_context(token tok) {con.update(tok.type);}
  int context_depth() const {return con.depth();}
  bool context_first_bracket_char() const {
    return con.first_bracket_char();
  }
  bool context_after_bracket_negation() const {
    return con.after_bracket_negation();
  }

  void set_token(token& tok, value_type ch, token_type type);
  void set_token(token& tok, value_type ch) {
    set_token(tok, ch, token_type::LITERAL);
  }
  // This is meant to be called in the EXPRESSION context.
  bool unmatched_right_delimiter(token& tok); 
  void check_for_right_delimiters();
  void convert_control_char(token& tok);
};

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::get(token& out) {
  if (source.empty()) {
    check_for_right_delimiters();
    return false;
  }

  get(out, con.get());
  update_context(out);
  return !error();
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::check_for_right_delimiters() {
  if (context_depth() > 0) {
    set_error(error_type::error_paren);
  } 

  switch (con.get()) {
  case expression_context::BRACKET:
  case expression_context::COLLATE:
  case expression_context::EQUIV:
  case expression_context::CLASS:
    set_error(error_type::error_brack);
    break;
  case expression_context::REPLICATION_LOWER:
    set_error(error_type::error_brace);
    break;
  default:
    break;
  }
}

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::get(token& out, 
    expression_context::context context) {
  switch (context) {
  case expression_context::EXPRESSION:
    expression_get(out);
    break;
  case expression_context::REPLICATION_LOWER:
    replication_get(out);
    break;
  case expression_context::SUBEXPR:
    subexpr_get(out);
    break;
  case expression_context::BRACKET:
    bracket_get(out);
    break;
  case expression_context::COLLATE:
    collation_get(out, value_type('.'), token_type::R_COLLATE);
    break;
  case expression_context::EQUIV:
    collation_get(out, value_type('='), token_type::R_EQUIV);
    break;
  case expression_context::CLASS:
    collation_get(out, value_type(':'), token_type::R_CLASS);
    break;
  default:
    break;
  }
  return !error();
}

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::expression_get(token& out) {
  value_type ch;
  source.get(ch);
  
  if (ch == value_type('\\')) {
     expression_get_escaped(out);
  } else {
    auto type = table.special_value(ch);
    set_token(out, ch, type);
  }
  unmatched_right_delimiter(out);
  convert_control_char(out);
  return !error();
}

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::expression_get_escaped(token& out) {
  value_type escaped_char('\0');

  if (!source.get(escaped_char)) {
    set_error(error_type::error_escape);
    set_token(out, '\\');
    return false;
  } 

  auto type = table.escaped_value(escaped_char);
  // Only ECMAScript is happy to interpret escaped ordinary characters.
  if (type == token_type::LITERAL && 
      !ECMAScript() &&
      table.is_ordinary(escaped_char)) { 
    set_error(error_type::error_escape);
  } 
  set_token(out, escaped_char, type);
  return !error();
}

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::replication_get(token& out) {
  value_type first('\0');
  source.get(first);

  if (first == value_type(',')) {
    set_token(out, first, token_type::COUNT_SEP);
  } 
  else if (std::isdigit(first, traits_.getloc())) {
    set_token(out, first, token_type::DIGIT);
  }
  else if ((basic() || grep()) && first != value_type('\\')) {
    set_token(out, first);
    set_error(error_type::error_badbrace);
  }
  else {
    if (!basic() && !grep()) {
      source.putback(first);
    }
    if (!source.get(first) || first != value_type('}')) {
      set_token(out, first);
      set_error(error_type::error_badbrace);
    } else {
      set_token(out, first, token_type::R_BRACE);
    }
  }
  return !error();
}

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::subexpr_get(token& out) {
  value_type first('\0');
  value_type second('\0');
  source.get(first);
  // We are looking for a two char sequence ?X, where X stands for
  // either =,!, or :.
  if (first != value_type('?') || !source.get(second)) {
    source.putback(first);
    return expression_get(out);
  } 
  switch (second) {
  case value_type('='):
  case value_type('!'):
    set_token(out, second, token_type::ASSERTION);
    return true;
  case value_type(':'):
    set_token(out, second, token_type::NO_SUBEXP);
    return true;
  default:
    source.putback(second);
    source.putback(first);
    return expression_get(out);
  }
}

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::bracket_get(token& out) {
  value_type ch('\0');
  source.get(ch);
  switch (ch) {
  case value_type('['):
    return bracket_get_sub_bracket(out);
  case value_type('^'):
    if (context_first_bracket_char()) {
      set_token(out, ch, token_type::NEGATION);
    } else {
      set_token(out, ch);
    }
    break;
  case value_type(']'):
    if (context_first_bracket_char() || context_after_bracket_negation()) {
      set_token(out, ch);
    } else {
      set_token(out, ch, token_type::R_BRACKET);
    }
    break;
  case value_type('-'):
    if (context_first_bracket_char() || context_after_bracket_negation()) {
      set_token(out, ch);
    } else {
      set_token(out, ch, token_type::RANGE_DASH);
    }
    break;
  default:
    set_token(out, ch);
    break;
  }
  return true;
}

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::bracket_get_sub_bracket(token& out) {
  value_type ch('[');
  value_type second('\0');
  if (!source.get(second)) {
    set_error(error_type::error_brack);
    set_token(out, ch);
    return false;
  } 
  switch (second) {
  case value_type('.'):
    set_token(out, ch, token_type::L_COLLATE);
    break;
  case value_type(':'):
    set_token(out, ch, token_type::L_CLASS);
    break;
  case value_type('='):
    set_token(out, ch, token_type::L_EQUIV);
    break;
  default:
    source.putback(second);
    set_token(out, ch);
    break;
  }
  return true;
}

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::collation_get(token& out, 
    value_type closing_char, token_type closing_token) {
  value_type first('\0');
  value_type second('\0');
  source.get(first);
  if (first != closing_char || !source.get(second)) {
    set_token(out, first);
  } else if (second != value_type(']')) {
    source.putback(second);
    set_token(out, first);
  } else {
    set_token(out, first, closing_token);
  }
  return true;
}

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::unmatched_right_delimiter(token& tok) {
  if (context_depth() <= 0 && tok.type == token_type::R_PAREN) {
    // Extended REs convert mismatched right parens to literals.
    if (extended() || egrep()) {
      tok.type = token_type::LITERAL;
    } 
    else { 
      set_error(error_type::error_paren);
    }
  }
  else if (tok.type == token_type::R_BRACKET) {
    set_error(error_type::error_brack);
  } 
  else if (tok.type == token_type::R_BRACE) {
    set_error(error_type::error_brace);
  }
  return !error();
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::convert_control_char(token& tok) {
  if (tok.type == token_type::CONTROL_CHAR) {
    value_type ch = tok.ch;

    switch (ch) {
    case 'f':
      ch = '\f';
      break;
    case 'n':
      ch = '\n';
      break;
    case 'v':
      ch = '\v';
      break;
    case 'r':
      ch = '\r';
      break;
    case 't':
      ch = '\t';
      break;
    }
    set_token(tok, ch, token_type::LITERAL);
  }
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::set_token(token& tok, value_type ch,
    token_type type) {
  if (icase() && type == token_type::LITERAL) {
    tok = token(std::toupper(ch,traits_.getloc()), type);
  } else {
    tok = token(ch, type);
  }
}

}// namespace lex
#endif// _token_stream_impl_h_
