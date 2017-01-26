#ifndef _token_stream_impl_h_
#define _token_stream_impl_h_

#include "regex_range.h"
#include "context.h"
#include "regex_token.h"
#include "token_table.h"

#include <locale>
#include <regex>

namespace lex {

template <typename InputIt, typename Traits>
class token_stream_impl {
 public:
  using flag_type = std::regex_constants::syntax_option_type;
  using value_type = typename regex_range<InputIt>::value_type;
  using token = regex_token<value_type,Traits>;
  using locale_type = typename Traits::locale_type;

  token_stream_impl(InputIt b, InputIt e, flag_type f)
    : table {make_token_table<value_type>(f)},
      source(b,e),
      loc {Traits{}.getloc()},
      flag {f} {}

  bool empty() const {return source.empty();}
  void get(token& out);
 private:
  token_table<value_type> table;
  regex_range<InputIt> source;
  locale_type loc;
  context con;
  flag_type flag;

  void get(token& out, context::site site);
  void default_get(token& out);
  void default_get_escaped(token& out);
  void replication_get(token& out);
  void subexpr_get(token& out);
  void bracket_get(token& out);
  void bracket_get_sub_bracket(token& out);
  void collation_get(token& out, value_type closing_char, 
      token_type closing_token);

  token literal_token(value_type ch) const;

  bool pair_reader(value_type& out, value_type first, value_type second);

  bool flag_includes(flag_type f) const {return flag & f;}

  void update_context(token tok) {con.update(tok.type);}
  int context_depth() const {return con.depth();}
  context::site get_context() const {return con.get();}
  bool context_first_bracket_char() const {
    return con.first_bracket_char();
  }
  bool context_after_bracket_negation() const {
    return con.after_bracket_negation();
  }
};

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::get(token& out) {
  get(out, get_context());
  update_context(out);
}

template <typename InputIt, typename Traits>
void 
token_stream_impl<InputIt,Traits>::get(token& out, context::site site) {
  switch (site) {
  case context::DEFAULT:
    default_get(out);
    break;
  case context::REPLICATION:
    replication_get(out);
    break;
  case context::SUBEXPR:
    subexpr_get(out);
    break;
  case context::BRACKET:
    bracket_get(out);
    break;
  case context::COLLATE:
    collation_get(out, value_type('.'), token_type::R_COLLATE);
    break;
  case context::EQUIV:
    collation_get(out, value_type('='), token_type::R_EQUIV);
    break;
  case context::CLASS:
    collation_get(out, value_type(':'), token_type::R_CLASS);
    break;
  }
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::default_get(token& out) {
  value_type ch;
  source.get(ch);
  if (ch == value_type('\\')) {
    default_get_escaped(out);
  } else {
    auto val = table.special_value(ch);
    if (val == token_type::R_PAREN && context_depth() == 0) {
      val = token_type::LITERAL;
    }
    out = (val == token_type::LITERAL)? literal_token(ch): token(ch,val);
  }
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::default_get_escaped(token& out) {
  value_type escaped_char(0);
  if (!source.get(escaped_char)) {
    out = token(value_type('\\'));
  } else {
    auto val = table.escaped_value(escaped_char);
    if (val == token_type::LITERAL) {
      out = literal_token(escaped_char);
    }
    out = token(escaped_char, val);
  }
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::replication_get(token& out) {
  bool basic = flag_includes(flag_type::basic) || 
    flag_includes(flag_type::grep);
  value_type ch(0);
  if ((basic  && pair_reader(ch, value_type('\\'), value_type('}'))) ||
      (!basic && source.get(ch) && ch == value_type('}'))) {
    out = token(value_type('}'), token_type::R_BRACE);
  } else if (ch == value_type(',')) {
    out = token(ch, token_type::COUNT_SEP);
  } else if (std::isdigit(ch, loc)) {
    out = token(ch, token_type::DIGIT);
  } else {
    out = literal_token(ch);
  }
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::subexpr_get(token& out) {
  // This method is only necessary for ECMAScript.
  bool ECMAScript = flag & flag_type::ECMAScript;
  if (!ECMAScript) {
    default_get(out);
    return;
  }

  value_type first(0);
  value_type second(0);
  source.get(first);

  // We are looking for a two char sequence ?X, where X stands for
  // either =,!, or :.
  if (first != value_type('?') || !source.get(second)) {
    source.putback(first);
    default_get(out);
    return;
  } 
  switch (second) {
  case value_type('='):
  case value_type('!'):
    out = token(second, token_type::ASSERTION);
    break;
  case value_type(':'):
    out = token(second, token_type::NO_SUBEXP);
    break;
  default:
    source.putback(second);
    source.putback(first);
    default_get(out);
  }
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::bracket_get(token& out) {
  value_type ch(0);
  source.get(ch);
  switch (ch) {
  case value_type('['):
    bracket_get_sub_bracket(out);
    break;
  case value_type('^'):
    if (context_first_bracket_char()) {
      out = token(ch, token_type::NEGATION);
    } else {
      out = literal_token(ch);
    }
    break;
  case value_type(']'):
    if (context_first_bracket_char() || context_after_bracket_negation()) {
      out = literal_token(ch);
    } else {
      out = token(ch, token_type::R_BRACKET);
    }
    break;
  case value_type('-'):
    out = token(ch, token_type::RANGE_DASH);
    break;
  default:
    out = literal_token(ch);
    break;
  }
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::bracket_get_sub_bracket(token& out) {
  value_type ch('[');
  value_type second(0);
  if (!source.get(second)) {
    out = literal_token(ch);
    return;
  } 
  switch (second) {
  case value_type('.'):
    out = token(ch, token_type::L_COLLATE);
    break;
  case value_type(':'):
    out = token(ch, token_type::L_CLASS);
    break;
  case value_type('='):
    out = token(ch, token_type::L_EQUIV);
    break;
  default:
    source.putback(second);
    out = literal_token(ch);
    break;
  }
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::collation_get(token& out, 
    value_type closing_char, token_type closing_token) {
  value_type ch(0);
  if (pair_reader(ch, closing_char, value_type(']'))) {
    out = token(ch, closing_token);
  } else {
    out = literal_token(ch);
  }
}

template <typename InputIt, typename Traits>
typename token_stream_impl<InputIt,Traits>::token
token_stream_impl<InputIt,Traits>::literal_token(value_type ch) const {
  if (flag_includes(flag_type::icase)) {
    ch = std::toupper(ch, loc);
  }
  return token(ch, token_type::LITERAL);
}

template <typename InputIt, typename Traits>
bool token_stream_impl<InputIt,Traits>::pair_reader(value_type& out, 
    value_type first, value_type second) {
  source.get(out);
  if (out == first && !source.empty()) {
    value_type ch(0);
    source.get(ch);
    if (ch == second) {
      return true;
    } else {
      source.putback(second);
    }
  }
  return false;
}

}// namespace lex
#endif// _token_stream_impl_h_
