#ifndef regex_factory_h_
#define regex_factory_h_

#include "regex_factory_impl.h"

#include <regex>

namespace lex {

template <typename Char, typename ST, typename SA, 
          typename Traits = std::regex_traits<Char>>
matcher<Char> create_regex(const 
    std::basic_string<Char,ST,SA>& regex_str) {
  auto begin = regex_str.begin();
  auto end = regex_str.end();
  auto regex = 
    detail::extended_reg_exp<decltype(begin), Traits>(begin, end);
  if (begin != end) {
    throw regex_error("Invalid regex: could not read");
  }
  return regex;
}
}//namespace lex

#endif// regex_factory_h_
