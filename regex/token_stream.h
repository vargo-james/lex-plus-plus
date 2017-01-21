#ifndef _token_stream_h_
#define _token_stream_h_

#include "regex_range.h"
#include "regex_context.h"
#include "regex_token.h"
#include "simple_buffer.h"
#include "token_table.h"

#include <cctype>
#include <locale>
#include <functional>
#include <regex>


namespace lex {


template <typename InputIt, typename Traits>
class token_stream {
 public:
  using flag_type = std::regex_constants::syntax_option_type;
  using iterator = InputIt;
  using value_type = typename std::iterator_traits<InputIt>::value_type;
  using token = regex_token<value_type>;
  using buffer_type = simple_buffer<token>;
  using range_type = regex_range<InputIt>;

  token_stream(InputIt b, InputIt e, flag_type f)
    : flag {f},
      range(b,e) {}

  bool get(token& out);
  bool putback(token tok) {return buffer.set(tok);}

  void update_context(token tok) {context.update(tok.type);}

  bool empty() const {return buffer.empty() && range.empty();}
 private:
  flag_type flag;
  range_type range;
  buffer_type buffer;
  regex_context context;

  void get_from_range(token& out);
  void default_get(token& out);
  void replication_get(token& out);
  void subexpr_get(token& out);
  void bracket_get(token& out);
  void collation_get(token& out, value_type closing_char, 
      token_type closing_token);

  // This reads the next two elements from the range. If their values are
  // first, second respectively, then the function returns true. If there
  // are not two values available, or the two values do not match, then 
  // the second value is putback, out is set to the first value read, and 
  // the function returns false.
  bool pair_reader(value_type& out, value_type first, value_type second);

};

template <typename Out>
Out flagged_test(bool flag_condition, std::function<Out()> test, 
    std::function<Out()> alternate_test) {
  return flag_condition? test(): alternate_test();
}

template <typename InputIt, typename Traits>
bool token_stream<InputIt,Traits>::get(token& out) {
  if (buffer.get(out)) {
    return true;
  }
  if (range.empty()) return false;
  get_from_range(out);
  update_context(out);
  return true;
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::get_from_range(token& out) {
  switch (context.get()) {
  case regex_context::DEFAULT:
    default_get(out);
    break;
  case regex_context::REPLICATION:
    replication_get(out);
    break;
  case regex_context::SUBEXPR:
    subexpr_get(out);
    break;
  case regex_context::BRACKET:
    bracket_get(out);
    break;
  case regex_context::COLLATE:
    collation_get(out, value_type('.'), token_type::R_COLLATE);
    break;
  case regex_context::EQUIV:
    collation_get(out, value_type('='), token_type::R_EQUIV);
    break;
  case regex_context::CLASS:
    collation_get(out, value_type(':'), token_type::R_CLASS);
    break;
  default:
    break;
  }
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::default_get(token& out) {
  bool basic = flag & std::regex_constants::basic;
  bool grep = flag & std::regex_constants::grep;
  bool egrep = flag & std::regex_constants::egrep;

  basic = basic || grep;

  const auto basic_table = BRE_special_characters<value_type>();
  const auto extended_table = ERE_special_characters<value_type>();

  const token_table<value_type>& table = basic? basic_table: extended_table;

  value_type ch;
  range.get(ch);
  if (ch == value_type('\\')) {
    value_type escaped_char(0);
    if (!range.get(escaped_char)) {
      out = token(value_type('\\'));
    } else if (escaped_char == value_type('b')) {
      out = token(escaped_char, token_type::WORD_BOUNDARY);
    } else if (escaped_char == value_type('B')) {
      out = token(escaped_char, token_type::NEG_WORD_BOUNDARY);
    } else if (std::isdigit(escaped_char)) {
      out = token(escaped_char, token_type::BACK_REF);
    } else if (table.contains(escaped_char)) {
      out = token(escaped_char);
    } else if (basic) {
      out = token(escaped_char, extended_table.value(escaped_char));
    } else {
      out = token(escaped_char);
    }
  } else if (ch == '\n' && (grep || egrep)) {
    out = token(value_type('|'), token_type::ALTERNATION);
    return;
  } else {
    out = token(ch, table.value(ch));
  }
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::replication_get(token& out) {
  bool basic = flag & std::regex_constants::basic;

  value_type ch(0);
  if ((basic  && pair_reader(ch, value_type('\\'), value_type('}'))) ||
      (!basic && range.get(ch) && ch == value_type('}'))) {
    out = token(value_type('}'), token_type::R_BRACE);
  } else if (ch == value_type(',')) {
    out = token(ch, token_type::COUNT_SEP);
  } else if (std::isdigit(ch, Traits{}.getloc())) {
    out = token(ch, token_type::DIGIT);
  } else {
    out = token(ch);
  }
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::subexpr_get(token& out) {
  // This method is only necessary for ECMAScript.
  bool ECMAScript = flag & std::regex_constants::ECMAScript;
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
    out = token(second, token_type::POS_LOOKAHEAD);
  } else if (second == value_type('!')) {
    out = token(second, token_type::NEG_LOOKAHEAD);
  } else if (second == value_type(':')) {
    out = token(second, token_type::NO_SUBEXP);
  } else {
    range.putback(second);
    range.putback(first);
    default_get(out);
  } 
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::bracket_get(token& out) {
  value_type ch(0);
  range.get(ch);
  if (ch == value_type('[')) {
    value_type second(0);
    if (!range.get(second)) {
      out = token(ch);
    } else if (second == value_type('.')) {
      out = token(ch, token_type::L_COLLATE);
    } else if (second == value_type(':')) {
      out = token(ch, token_type::L_CLASS);
    } else if (second == value_type('=')) {
      out = token(ch, token_type::L_EQUIV);
    } else {
      range.putback(second);
      out = token(ch);
    }
  } else if (ch == value_type('^')) {
    out = token(ch, token_type::NEGATION);
  } else if (ch == value_type('-')) {
    out = token(ch, token_type::RANGE_DASH);
  } else if (ch == value_type(']')) {
    out = token(ch, token_type::R_BRACKET);
  } else {
    out = token(ch);
  }
}

template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::collation_get(token& out, 
    value_type closing_char, token_type closing_token) {
  value_type ch(0);
  if (pair_reader(ch, closing_char, value_type(']'))) {
    out = token(ch, closing_token);
  } else {
    out = token(ch);
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
