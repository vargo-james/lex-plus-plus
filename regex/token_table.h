#ifndef _token_table_h_
#define _token_table_h_

#include "regex_token.h"

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

namespace lex {

template <typename CharT>
class token_table {
 public:
  using key_type = CharT;
  using value_type = std::pair<CharT,token_type>;

  token_table(const std::initializer_list<value_type>& init): table_(init) {}

  bool contains(key_type key) const;
  token_type value(key_type key) const;

 private:
  std::vector<value_type> table_;
};

template <typename CharT>
bool token_table<CharT>::contains(key_type key) const {
  using std::begin;
  using std::end;
  using std::find_if;

  auto it = find_if(begin(table_), end(table_), 
      [key](value_type val) {
        return val.first == key;
      });

  return it != end(table_);
}

template <typename CharT>
token_type token_table<CharT>::value(key_type key) const {
  using std::begin;
  using std::end;
  using std::find_if;

  auto it = find_if(begin(table_), end(table_), 
      [key](value_type val) {
        return val.first == key;
      });

  if (it == end(table_)) return token_type::LITERAL;
  return it->second;
}

template <typename CharT>
token_table<CharT> BRE_special_characters() {
  return token_table<CharT> {
    {CharT('.'), token_type::ANY_CHAR},
    {CharT('['), token_type::L_BRACKET},
    {CharT('*'), token_type::REPLICATION},
    {CharT('^'), token_type::L_ANCHOR},
    {CharT('$'), token_type::R_ANCHOR}
  };
}

template <typename CharT>
token_table<CharT> ERE_special_characters() {
  return token_table<CharT> {
    {CharT('.'), token_type::ANY_CHAR},
    {CharT('['), token_type::L_BRACKET},
    {CharT(']'), token_type::R_BRACKET},
    {CharT('('), token_type::L_PAREN},
    {CharT(')'), token_type::R_PAREN},
    {CharT('{'), token_type::L_BRACE},
    {CharT('}'), token_type::R_BRACE},
    {CharT('*'), token_type::REPLICATION},
    {CharT('+'), token_type::REPLICATION},
    {CharT('?'), token_type::REPLICATION},
    {CharT('|'), token_type::ALTERNATION},
    {CharT('^'), token_type::L_ANCHOR},
    {CharT('$'), token_type::R_ANCHOR}
  };
}
}//namespace lex
#endif// _token_table_h_
