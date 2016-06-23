#ifndef _translator_h_
#define _translator_h_

#include "regex_expression.h"
#include "simple_regex.h"

#include <functional>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace token_iterator {

template <typename Char, typename F>
class translator {
 public:
  using char_type        = Char;
  using regex_type       = simple_regex<char_type>;
  using string_type      = std::basic_string<char_type>;
  using function_type    = F;
  using proto_value_type = std::pair<string_type, function_type>;
  using value_type       = std::pair<regex_type, function_type>;
  using table_type       = std::vector<value_type>; 
  using iterator         = typename table_type::iterator;;

  // Iterator::value_type must be proto_value_type.
  template <typename Iterator>
  translator(Iterator begin, Iterator end);
  translator(
      const std::initializer_list<proto_value_type>& l)
    : translator(l.begin(), l.end()) {}
  translator(const std::vector<proto_value_type>& v)
    : translator(v.begin(), v.end()) {}

  iterator begin() {return table.begin();} 
  iterator end() {return table.end();}
 private:
  table_type table;
};

template <typename Char, typename F>
template <typename Iterator>
translator<Char,F>::translator(Iterator begin, Iterator end) {
  using std::make_pair;

  for (auto it = begin; it != end; ++it) {
    table.push_back(make_pair(create_regex(it->first), it->second));
  }
}

}//namespace token_iterator
#endif// _translator_h_
