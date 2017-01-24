#ifndef _token_stream_h_
#define _token_stream_h_

#include "regex_range.h"
#include "context.h"
#include "regex_token.h"
#include "simple_buffer.h"
#include "token_table.h"

#include <functional>
#include <iterator>
#include <locale>
#include <regex>


namespace lex {


template <typename InputIt, typename Traits>
class token_stream {
 public:
  using flag_type = std::regex_constants::syntax_option_type;
  using iterator = InputIt;
  using value_type = typename std::iterator_traits<InputIt>::value_type;
  using token = regex_token<value_type,Traits>;
  using buffer_type = simple_buffer<token>;
  using range_type = regex_range<InputIt,Traits>;

  token_stream(InputIt b, InputIt e, flag_type f)
    : flag {f},
      range(b,e) {}

  bool get(token& out);
  bool putback(token tok) {return buffer.set(tok);}

  void update_context(token tok) {context_i.update(tok.type);}

  bool empty() const {return buffer.empty() && range.empty();}
 private:
  flag_type flag;
  range_type range;
  buffer_type buffer;
  context context_i;
  Traits traits_i;

  void get_from_range(token& out);
  void default_get(token& out);
  void default_get_escaped(token& out);
  void replication_get(token& out);
  void subexpr_get(token& out);
  void bracket_get(token& out);
  void collation_get(token& out, value_type closing_char, 
      token_type closing_token);
  token literal_token(value_type ch);

  // This reads the next two elements from the range. If their values are
  // first, second respectively, then the function returns true. If there
  // are not two values available, or the two values do not match, then 
  // the second value is putback, out is set to the first value read, and 
  // the function returns false.
  bool pair_reader(value_type& out, value_type first, value_type second);

};


template <typename InputIt, typename Traits>
void token_stream<InputIt,Traits>::default_get_escaped(token& out) {
  bool basic = flag & std::regex_constants::basic;
  bool ECMAScript = flag & std::regex_constants::ECMAScript;

  const auto basic_table = BRE_special_characters<value_type>();
  const auto extended_table = ERE_special_characters<value_type>();
  const token_table<value_type>& table = basic? basic_table: extended_table;

  value_type escaped_char(0);
  if (!range.get(escaped_char)) {
    out = token(value_type('\\'));
  } else if (ECMAScript && escaped_char == value_type('b')) {
    out = token(escaped_char, token_type::WORD_BOUNDARY);
  } else if (ECMAScript && escaped_char == value_type('B')) {
    out = token(escaped_char, token_type::NEG_WORD_BOUNDARY);
  } else if (std::isdigit(escaped_char, traits_i.getloc())) {
    out = token(escaped_char, token_type::BACK_REF);
  } else if (table.contains(escaped_char)) {
    out = literal_token(escaped_char);
  } else if (basic) {
    auto val = extended_table.value(escaped_char);
    out = (val == token_type::LITERAL)? literal_token(escaped_char) :
      token(escaped_char, val);
  } else {
    out = literal_token(escaped_char);
  }
}

template <typename InputIt, typename Traits>
typename token_stream<InputIt,Traits>::token
token_stream<InputIt,Traits>::literal_token(value_type ch) {
  bool icase = flag & std::regex_constants::icase;

  if (icase) {
    ch = std::toupper(ch, traits_i.getloc());
  }
  return token(ch, token_type::LITERAL);
}

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
  switch (context_i.get()) {
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
    default_get_escaped(out);
    /*
    value_type escaped_char(0);
    if (!range.get(escaped_char)) {
      out = token(value_type('\\'));
    } else if (escaped_char == value_type('b')) {
      out = token(escaped_char, token_type::WORD_BOUNDARY);
    } else if (escaped_char == value_type('B')) {
      out = token(escaped_char, token_type::NEG_WORD_BOUNDARY);
    } else if (std::isdigit(escaped_char, traits_i.getloc())) {
      out = token(escaped_char, token_type::BACK_REF);
    } else if (table.contains(escaped_char)) {
      out = literal_token(escaped_char);
    } else if (basic) {
      auto val = extended_table.value(escaped_char);
      out = (val == token_type::LITERAL)? literal_token(escaped_char) :
        token(escaped_char, val);
    } else {
      out = literal_token(escaped_char);
    }
    */
  } else if (ch == '\n' && (grep || egrep)) {
    out = token(value_type('|'), token_type::ALTERNATION);
    return;
  } else {
    auto val = table.value(ch);
    out = (val == token_type::LITERAL)? literal_token(ch): token(ch,val);
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
  } else if (std::isdigit(ch, traits_i.getloc())) {
    out = token(ch, token_type::DIGIT);
  } else {
    out = literal_token(ch);
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
  } else if (ch == value_type('^')) {
    out = token(ch, token_type::NEGATION);
  } else if (ch == value_type('-')) {
    out = token(ch, token_type::RANGE_DASH);
  } else if (ch == value_type(']')) {
    out = token(ch, token_type::R_BRACKET);
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
