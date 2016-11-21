#ifndef _translator_h_
#define _translator_h_

#include "regex_factory.h"
#include "matcher/matcher.h"

#include <functional>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace lex {

// The type L must provide a function_type and a char_type.
template <typename Regex, typename F>
class translator {
 public:
  using char_type        = typename Regex::char_type;
  using regex_type       = matcher<char_type>;
  using string_type      = std::basic_string<char_type>;
  using function_type    = F;
  using proto_value_type = std::pair<string_type, function_type>;
  using value_type       = std::pair<regex_type, function_type>;
  using table_type       = std::vector<value_type>; 
  using iterator         = typename table_type::iterator;

  // Iterator::value_type must be proto_value_type.
  // The following constructors make a translator object from lists
  // using regex strings (e.g. "[ab]{2,4}").
  template <typename Iterator, 
            typename = enable_iterator_t<Iterator,proto_value_type>>
  translator(Iterator begin, Iterator end);
  translator(
      const std::initializer_list<proto_value_type>& l)
    : translator(l.begin(), l.end()) {}
  translator(const std::vector<proto_value_type>& v)
    : translator(v.begin(), v.end()) {}
  // The following constructors make a translator object from lists
  // using pre-constructed matcher<char_type> objects.
  translator(const std::initializer_list<value_type>& l)
    : table(l) {}
  translator(const std::vector<value_type>& v)
    : table(v) {}

  iterator begin() {return table.begin();} 
  iterator end() {return table.end();}
 private:
  table_type table;
};

template <typename Regex, typename F>
template <typename Iterator, typename>
translator<Regex,F>::translator(Iterator begin, Iterator end) {
  using std::make_pair;
  for (auto it = begin; it != end; ++it) {
    table.push_back(make_pair(create_regex(it->first), it->second));
  }
}

}//namespace lex
#endif// _translator_h_
