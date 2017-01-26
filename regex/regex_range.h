#ifndef _regex_range_h_
#define _regex_range_h_

#include "simple_buffer.h"

#include <iterator>

namespace lex {

template <typename InputIt, unsigned N = 2>
class regex_range {
 public:
  using value_type = typename std::iterator_traits<InputIt>::value_type;

  regex_range(InputIt b, InputIt e) : current {b}, end {e} {}

  bool get(value_type& ch);
  bool putback(value_type ch);

  bool empty() const;
 private:
  InputIt current;
  InputIt end;
  simple_buffer<value_type,N> buffer;
};

template <typename InputIt, unsigned N>
bool regex_range<InputIt,N>::empty() const {
  return buffer.empty() && current == end;
}

template <typename InputIt, unsigned N>
bool regex_range<InputIt,N>::putback(value_type ch) {
  return buffer.push(ch);
}

template <typename InputIt, unsigned N>
bool regex_range<InputIt,N>::get(value_type& ch) {
  if (!buffer.get(ch)) {
    if (current == end) return false;
    ch = *current++;
    return true;
  }
  return true;
}



}//namespace lex
#endif// _regex_range_h_
