#ifndef _token_stream_h_
#define _token_stream_h_

#include "regex_range.h"
#include "context.h"
#include "regex_token.h"
#include "simple_buffer.h"
#include "token_table.h"

#include "token_stream_impl.h"

#include <functional>
#include <iterator>
#include <locale>
#include <regex>


namespace lex {

template <typename InputIt, typename Traits>
class new_token_stream {
 public:
  using value_type = typename token_stream_impl<InputIt,Traits>::value_type;
  using token = regex_token<value_type,Traits>;
  using flag_type = typename token_stream_impl<InputIt,Traits>::flag_type;

  new_token_stream(InputIt b, InputIt e, flag_type f)
    : impl(b,e,f) {} 

  bool get(token& out);
  bool putback(token tok) {return buffer.set(tok);}
  bool empty() const {return buffer.empty() && source_empty();}

 private:
  simple_buffer<value_type> buffer;
  token_stream_impl<InputIt,Traits> impl;

  bool source_empty() const {return impl.empty();}
  void get_from_source(token& out) {impl.get(out);} 
};

template <typename InputIt, typename Traits>
bool new_token_stream<InputIt,Traits>::get(token& out) {
  if (buffer.get(out)) {
    return true;
  }
  if (source_empty()) return false;
  get_from_source(out);
  return true;
}

template <typename InputIt, typename Traits>
class token_stream {
 public:
  using flag_type = std::regex_constants::syntax_option_type;
  using value_type = typename std::iterator_traits<InputIt>::value_type;
  using token = regex_token<value_type,Traits>;

  token_stream(InputIt b, InputIt e, flag_type f)
    : flag {f},
      range(b,e),
      loc {Traits{}.getloc()} {}

  bool get(token& out);
  bool putback(token tok) {return buffer.set(tok);}

  bool empty() const {return buffer.empty() && range.empty();}
 private:
  using locale_type = typename Traits::locale_type;
  using table_type = token_table<value_type>;

  flag_type flag;
  regex_range<InputIt,Traits> range;
  simple_buffer<token> buffer;
  context context_i;
  locale_type loc;

  bool flag_includes(flag_type f) const {return flag & f;}

  void get_from_range(token& out);
  void default_get(token& out);
  void default_get_escaped(token& out);
  void replication_get(token& out);
  void subexpr_get(token& out);
  void bracket_get(token& out);
  void bracket_get_sub_bracket(token& out);
  void collation_get(token& out, value_type closing_char, 
      token_type closing_token);
  token literal_token(value_type ch);

  // This reads the next two elements from the range. If their values are
  // first, second respectively, then the function returns true. If there
  // are not two values available, or the two values do not match, then 
  // the second value is putback, out is set to the first value read, and 
  // the function returns false.
  bool pair_reader(value_type& out, value_type first, value_type second);

  // Context observers.
  void update_context(token tok) {context_i.update(tok.type);}
  int context_depth() const {return context_i.depth();}
  auto get_context() const {return context_i.get();}
  bool context_first_bracket_char() const {
    return context_i.first_bracket_char();
  }
  bool context_after_bracket_negation() const {
    return context_i.after_bracket_negation();
  }
};


template <typename InputIt, typename Traits>
typename token_stream<InputIt,Traits>::token
token_stream<InputIt,Traits>::literal_token(value_type ch) {
  if (flag_includes(flag_type::icase)) {
    ch = std::toupper(ch, loc);
  }
  return token(ch, token_type::LITERAL);
}

template <typename InputIt, typename Traits>
bool token_stream<InputIt,Traits>::get(token& out) {
  if (buffer.get(out)) {
    return true;
  }
  if (range.empty()) return false;
  get_from_range(out);
  return true;
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::get_from_range(token& out) {
  switch (get_context()) {
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
  default:
    break;
  }
  update_context(out);
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::default_get(token& out) {
  value_type ch;
  range.get(ch);
  if (ch == value_type('\\')) {
    default_get_escaped(out);
  } else {
    const auto special_table = special_character_table<value_type>(flag);
    auto val = special_table.value(ch);
    if (val == token_type::R_PAREN && context_depth() == 0) {
      val = token_type::LITERAL;
    }
    out = (val == token_type::LITERAL)? literal_token(ch): token(ch,val);
  }
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::default_get_escaped(token& out) {
  const auto escape_table = escape_character_table<value_type>(flag);
  value_type escaped_char(0);
  if (!range.get(escaped_char)) {
    out = token(value_type('\\'));
  } else {
    auto val = escape_table.value(escaped_char);
    if (val == token_type::LITERAL) {
      out = literal_token(escaped_char);
    }
    out = token(escaped_char, val);
  }
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::replication_get(token& out) {
  bool basic = flag & flag_type::basic;

  value_type ch(0);
  if ((basic  && pair_reader(ch, value_type('\\'), value_type('}'))) ||
      (!basic && range.get(ch) && ch == value_type('}'))) {
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
void token_stream<InputIt,Traits>::subexpr_get(token& out) {
  // This method is only necessary for ECMAScript.
  bool ECMAScript = flag & flag_type::ECMAScript;
  if (!ECMAScript) {
    default_get(out);
    return;
  }

  value_type first(0);
  value_type second(0);
  range.get(first);

  // We are looking for a two char sequence ?X, where X stands for
  // either =,!, or :.
  if (first != value_type('?') || !range.get(second)) {
    range.putback(first);
    default_get(out);
  } else if (second == value_type('=')) {
    out = token(second, token_type::ASSERTION);
  } else if (second == value_type('!')) {
    out = token(second, token_type::ASSERTION);
  } else if (second == value_type(':')) {
    out = token(second, token_type::NO_SUBEXP);
  } else {
    range.putback(second);
    range.putback(first);
    default_get(out);
  } 
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::bracket_get_sub_bracket(token& out) {
  value_type ch('[');
  value_type second(0);
  if (!range.get(second)) {
    out = literal_token(ch);
  } else if (second == value_type('.')) {
    out = token(ch, token_type::L_COLLATE);
  } else if (second == value_type(':')) {
    out = token(ch, token_type::L_CLASS);
  } else if (second == value_type('=')) {
    out = token(ch, token_type::L_EQUIV);
  } else {
    range.putback(second);
    out = literal_token(ch);
  }
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::bracket_get(token& out) {
  value_type ch(0);
  range.get(ch);
  if (ch == value_type('[')) {
    bracket_get_sub_bracket(out);
    /*
    value_type second(0);
    if (!range.get(second)) {
      out = literal_token(ch);
    } else if (second == value_type('.')) {
      out = token(ch, token_type::L_COLLATE);
    } else if (second == value_type(':')) {
      out = token(ch, token_type::L_CLASS);
    } else if (second == value_type('=')) {
      out = token(ch, token_type::L_EQUIV);
    } else {
      range.putback(second);
      out = literal_token(ch);
    }
    */
  } else if (ch == value_type('^')) {
    if (context_first_bracket_char()) {
      out = token(ch, token_type::NEGATION);
    } else {
      out = literal_token(ch);
    }
  } else if (ch == value_type(']')) {
    if (context_first_bracket_char() || context_after_bracket_negation()) {
      out = literal_token(ch);
    } else {
      out = token(ch, token_type::R_BRACKET);
    }
  } else if (ch == value_type('-')) {
    out = token(ch, token_type::RANGE_DASH);
  } else {
    out = literal_token(ch);
  }
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::collation_get(token& out, 
    value_type closing_char, token_type closing_token) {
  value_type ch(0);
  if (pair_reader(ch, closing_char, value_type(']'))) {
    out = token(ch, closing_token);
  } else {
    out = literal_token(ch);
  }
}

template <typename InputIt, typename Traits>
bool token_stream<InputIt,Traits>::pair_reader(value_type& out, value_type first, 
    value_type second) {
  range.get(out);
  if (out == first && !range.empty()) {
    value_type ch(0);
    range.get(ch);
    if (ch == second) {
      return true;
    } else {
      range.putback(second);
    }
  }
  return false;
}

}//namespace lex
#endif// _token_stream_h_
