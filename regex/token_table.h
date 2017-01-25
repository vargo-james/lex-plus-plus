/*
 * Notes:
 *  The UNICODE_ESCAPE is hidden.
 */
 
#ifndef _token_table_h_
#define _token_table_h_

#include "regex_token.h"

#include <algorithm>
#include <iterator>
#include <regex>
#include <utility>
#include <vector>

namespace lex {

template <typename CharT> 
class token_table {
 public:
  using key_type = const CharT;
  using entry_type = std::pair<key_type,token_type>;

  token_table(const std::initializer_list<entry_type>& lit_init)
    : entries(lit_init) {}

  bool contains(key_type key) const;
  token_type value(key_type key) const;

  void insert(const entry_type& l_val);
  void join(const token_table& other);

 private:
  std::vector<entry_type> entries;
};

template <typename CharT>
void token_table<CharT>::insert(const entry_type& val) {
  if (!contains(val.first)) {
    entries.push_back(val);
  }
}
  
template <typename CharT>
void token_table<CharT>::join(const token_table& other) {
  for (const auto& val: other.entries) {
    insert(val);
  }
}

template <typename CharT>
bool token_table<CharT>::contains(key_type key) const {
  using std::begin;
  using std::end;
  using std::find_if;

  auto it = find_if(begin(entries), end(entries), 
      [key](const auto& val) {
        return val.first == key;
      });

  return it != end(entries);
}

template <typename CharT>
token_type token_table<CharT>::value(key_type key) const {
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
token_table<CharT> basic_special_characters() {
  return  token_table<CharT>({
    {CharT('.'), token_type::ANY_CHAR},
    {CharT('['), token_type::L_BRACKET},
    {CharT('*'), token_type::REPLICATION},
    {CharT('^'), token_type::ASSERTION},
    {CharT('$'), token_type::ASSERTION},
  }); 
}

template <typename CharT>
token_table<CharT> basic_escape_characters() {
  return token_table<CharT>({
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
token_table<CharT> extended_special_characters() {
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
  }); 
}

template <typename CharT>
token_table<CharT> extended_escape_characters() {
  return token_table<CharT>({
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
token_table<CharT> grep_special_characters() {
  auto table = basic_special_characters<CharT>();
  table.insert({CharT('\n'), token_type::ALTERNATION});
  return table;
}

template <typename CharT>
token_table<CharT> grep_escape_characters() {
  return basic_escape_characters<CharT>();
}

template <typename CharT>
token_table<CharT> egrep_special_characters() {
  auto ext_table = extended_special_characters<CharT>();
  ext_table.insert({CharT('\n'), token_type::ALTERNATION});
  return ext_table;
}

template <typename CharT>
token_table<CharT> egrep_escape_characters() {
  return extended_escape_characters<CharT>();
}

template <typename CharT>
token_table<CharT> ECMAScript_special_characters() {
  return extended_special_characters<CharT>();
}

template <typename CharT>
token_table<CharT> ECMAScript_escape_characters() {
  auto table = extended_escape_characters<CharT>();
  table.join({
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
  return table;
}

template <typename CharT>
token_table<CharT> 
special_character_table(std::regex_constants::syntax_option_type flag) {
  using flag_type = std::regex_constants::syntax_option_type;
  if (flag & flag_type::basic) {
    return basic_special_characters<CharT>();
  }
  if (flag & flag_type::grep) {
    return grep_special_characters<CharT>();
  }
  if (flag & flag_type::egrep) {
    return egrep_special_characters<CharT>();
  }
  if (flag & flag_type::extended) {
    return extended_special_characters<CharT>();
  }
  if (flag & flag_type::ECMAScript) {
    return ECMAScript_special_characters<CharT>();
  }
  return basic_special_characters<CharT>();
}

template <typename CharT>
token_table<CharT> 
escape_character_table(std::regex_constants::syntax_option_type flag) {
  using flag_type = std::regex_constants::syntax_option_type;
  if (flag & flag_type::basic) {
    return basic_escape_characters<CharT>();
  }
  if (flag & flag_type::grep) {
    return grep_escape_characters<CharT>();
  }
  if (flag & flag_type::egrep) {
    return egrep_escape_characters<CharT>();
  }
  if (flag & flag_type::extended) {
    return extended_escape_characters<CharT>();
  }
  if (flag & flag_type::ECMAScript) {
    return ECMAScript_escape_characters<CharT>();
  }
  return basic_escape_characters<CharT>();
}

}//namespace lex
#endif// _token_table_h_
