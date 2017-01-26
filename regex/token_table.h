/*
 * A table tells a token_stream how to interpret characters outside of a 
 * bracket expression and outside of braces. That is, it interprets all the
 * special characters and escape characters in the regex.
 *
 * Notes:
 *  The UNICODE_ESCAPE is hidden.
 */
 
#ifndef _token_table_h_
#define _token_table_h_

#include "regex_token.h"
#include "regex_types.h"

#include <algorithm>
#include <initializer_list>
#include <regex>
#include <utility>
#include <vector>

namespace lex {

template <typename CharT> 
class token_table {
 public:
  using key_type = CharT;
  using entry_type = std::pair<key_type,token_type>;

  token_table(const std::initializer_list<entry_type>& spec_init,
      const std::initializer_list<entry_type>& esc_init)
    : specials(spec_init),
      escapes(esc_init) {}

  token_type special_value(key_type key) const {return value(key,specials);}
  token_type escaped_value(key_type key) const {return value(key,escapes);}

  void insert_special(const entry_type& val) {specials.push_back(val);}
  void insert_escaped(const entry_type& val) {escapes.push_back(val);}

 private:
  std::vector<entry_type> specials;
  std::vector<entry_type> escapes;

  token_type value(key_type, const std::vector<entry_type>& entries) const;
};

template <typename CharT>
token_type token_table<CharT>::value(key_type key, 
    const std::vector<entry_type>& entries) const {
  using std::begin;
  using std::end;
  using std::find_if;

  auto it = find_if(begin(entries), end(entries), 
      [key](const auto& val) {
        return val.first == key;
      });

  if (it != end(entries)) return it->second;

  return token_type::LITERAL;
}

template <typename CharT>
token_table<CharT> basic_table() {
  return  token_table<CharT>({
    {CharT('.'), token_type::ANY_CHAR},
    {CharT('['), token_type::L_BRACKET},
    {CharT('*'), token_type::REPLICATION},
    {CharT('^'), token_type::ASSERTION},
    {CharT('$'), token_type::ASSERTION},
  }, {
    {CharT('('), token_type::L_PAREN},
    {CharT(')'), token_type::R_PAREN},
    {CharT('{'), token_type::L_BRACE},
    {CharT('+'), token_type::REPLICATION},
    {CharT('?'), token_type::REPLICATION},
    {CharT('|'), token_type::ALTERNATION},
    {CharT('0'), token_type::BACK_REF},
    {CharT('1'), token_type::BACK_REF},
    {CharT('2'), token_type::BACK_REF},
    {CharT('3'), token_type::BACK_REF},
    {CharT('4'), token_type::BACK_REF},
    {CharT('5'), token_type::BACK_REF},
    {CharT('6'), token_type::BACK_REF},
    {CharT('7'), token_type::BACK_REF},
    {CharT('8'), token_type::BACK_REF},
    {CharT('9'), token_type::BACK_REF},
  }); 
}

template <typename CharT>
token_table<CharT> extended_table() {
  return  token_table<CharT>({
    {CharT('.'), token_type::ANY_CHAR},
    {CharT('['), token_type::L_BRACKET},
    {CharT('*'), token_type::REPLICATION},
    {CharT('^'), token_type::ASSERTION},
    {CharT('$'), token_type::ASSERTION},
    {CharT('('), token_type::L_PAREN},
    {CharT(')'), token_type::R_PAREN},
    {CharT('{'), token_type::L_BRACE},
    {CharT('+'), token_type::REPLICATION},
    {CharT('?'), token_type::REPLICATION},
    {CharT('|'), token_type::ALTERNATION},
  }, {
    {CharT('0'), token_type::BACK_REF},
    {CharT('1'), token_type::BACK_REF},
    {CharT('2'), token_type::BACK_REF},
    {CharT('3'), token_type::BACK_REF},
    {CharT('4'), token_type::BACK_REF},
    {CharT('5'), token_type::BACK_REF},
    {CharT('6'), token_type::BACK_REF},
    {CharT('7'), token_type::BACK_REF},
    {CharT('8'), token_type::BACK_REF},
    {CharT('9'), token_type::BACK_REF},
  }); 
}

template <typename CharT>
token_table<CharT> grep_table() {
  auto table = basic_table<CharT>();
  table.insert_special({CharT('\n'), token_type::ALTERNATION});
  return table;
}

template <typename CharT>
token_table<CharT> egrep_table() {
  auto table = extended_table<CharT>();
  table.insert_special({CharT('\n'), token_type::ALTERNATION});
  return table;
}

template <typename CharT>
token_table<CharT> ECMAScript_table() {
  return  token_table<CharT>({
    {CharT('.'), token_type::ANY_CHAR},
    {CharT('['), token_type::L_BRACKET},
    {CharT('*'), token_type::REPLICATION},
    {CharT('^'), token_type::ASSERTION},
    {CharT('$'), token_type::ASSERTION},
    {CharT('('), token_type::L_PAREN},
    {CharT(')'), token_type::R_PAREN},
    {CharT('{'), token_type::L_BRACE},
    {CharT('+'), token_type::REPLICATION},
    {CharT('?'), token_type::REPLICATION},
    {CharT('|'), token_type::ALTERNATION},
  }, {
    {CharT('0'), token_type::BACK_REF},
    {CharT('1'), token_type::BACK_REF},
    {CharT('2'), token_type::BACK_REF},
    {CharT('3'), token_type::BACK_REF},
    {CharT('4'), token_type::BACK_REF},
    {CharT('5'), token_type::BACK_REF},
    {CharT('6'), token_type::BACK_REF},
    {CharT('7'), token_type::BACK_REF},
    {CharT('8'), token_type::BACK_REF},
    {CharT('9'), token_type::BACK_REF},
    {CharT('w'), token_type::CHAR_CLASS},
    {CharT('W'), token_type::CHAR_CLASS},
    {CharT('d'), token_type::CHAR_CLASS},
    {CharT('D'), token_type::CHAR_CLASS},
    {CharT('s'), token_type::CHAR_CLASS},
    {CharT('S'), token_type::CHAR_CLASS},
    {CharT('l'), token_type::CHAR_CLASS},
    {CharT('L'), token_type::CHAR_CLASS},
    {CharT('u'), token_type::CHAR_CLASS},
    {CharT('U'), token_type::CHAR_CLASS},
    {CharT('f'), token_type::CONTROL_CHAR},
    {CharT('n'), token_type::CONTROL_CHAR},
    {CharT('r'), token_type::CONTROL_CHAR},
    {CharT('t'), token_type::CONTROL_CHAR},
    {CharT('v'), token_type::CONTROL_CHAR},
    {CharT('c'), token_type::CONTROL},
    {CharT('x'), token_type::HEX_ESCAPE},
    {CharT('u'), token_type::UNICODE_ESCAPE},
    {CharT('c'), token_type::CONTROL},
    {CharT('b'), token_type::ASSERTION},
    {CharT('B'), token_type::ASSERTION}
  }); 
}

template <typename CharT>
token_table<CharT> 
make_token_table(regex_constants::syntax_option_type flag) {
  using flag_type = regex_constants::syntax_option_type;
  if (flag & flag_type::basic) {
    return basic_table<CharT>();
  }
  if (flag & flag_type::grep) {
    return grep_table<CharT>();
  }
  if (flag & flag_type::egrep) {
    return egrep_table<CharT>();
  }
  if (flag & flag_type::extended) {
    return extended_table<CharT>();
  }
  if (flag & flag_type::ECMAScript) {
    return ECMAScript_table<CharT>();
  }
  return basic_table<CharT>();
}

}//namespace lex
#endif// _token_table_h_
