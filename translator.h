#ifndef _translator_h_
#define _translator_h_

#include "regex_expression.h"
#include "simple_regex.h"

#include <functional>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace lex {

template <typename L>
class translator {
 public:
  using char_type        = typename L::char_type;
  using regex_type       = simple_regex<char_type>;
  using string_type      = std::basic_string<char_type>;
  using function_type    = typename L::function_type;
  using proto_value_type = std::pair<string_type, function_type>;
  using value_type       = std::pair<regex_type, function_type>;
  using table_type       = std::vector<value_type>; 
  using iterator         = typename table_type::iterator;

  // Iterator::value_type must be proto_value_type.
  // The following constructors make a translator object from lists
  // using regex strings (e.g. "[ab]{2,4}").
  template <typename Iterator>
  translator(Iterator begin, Iterator end);
  translator(
      const std::initializer_list<proto_value_type>& l)
    : translator(l.begin(), l.end()) {}
  translator(const std::vector<proto_value_type>& v)
    : translator(v.begin(), v.end()) {}
  // The following constructors make a translator object from lists
  // using pre-constructed simple_regex<char_type> objects.
  translator(const std::initializer_list<value_type>& l)
    : table(l) {}
  translator(const std::vector<value_type>& v)
    : table(v) {}

  iterator begin() {return table.begin();} 
  iterator end() {return table.end();}
 private:
  table_type table;
};

template <typename L>
template <typename Iterator>
translator<L>::translator(Iterator begin, Iterator end) {
  using std::make_pair;

  for (auto it = begin; it != end; ++it) {
    table.push_back(make_pair(create_regex(it->first), it->second));
  }
}

}//namespace lex
#endif// _translator_h_
