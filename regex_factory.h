#ifndef regex_factory_h_
#define regex_factory_h_

#include "regex_factory_impl.h"

namespace lex {

template <typename Char>
simple_regex<Char> create_regex(const std::basic_string<Char>& regex_str) {
  auto begin = regex_str.begin();
  auto end = regex_str.end();
  auto regex = detail::extended_reg_exp(begin, end);
  if (begin != end) {
    throw regex_error("Invalid regex: could not read");
  }
  return regex;
}
}//namespace lex

#endif// regex_factory_h_
