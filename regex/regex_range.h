#ifndef _regex_range_h_
#define _regex_range_h_

#include <iterator>
#include <queue>

namespace lex {

template <typename InputIt>
class regex_range {
 public:
  using iterator = InputIt;
  using value_type = typename std::iterator_traits<iterator>::value_type;

  regex_range(iterator b, iterator e) : current {b}, end {e} {}

  bool get(value_type& ch);
  void putback(value_type ch);

  bool empty() const {return buffer.empty() && current == end;}
 private:
  iterator current;
  iterator end;
  std::queue<value_type> buffer;
};

template <typename InputIt>
bool regex_range<InputIt>::get(value_type& ch) {
  if (!buffer.empty()) {
    ch = buffer.front();
    buffer.pop();
    return true;
  } else if (current != end) {
    ch = *current++;
    return true;
  }
  return false;
}

template <typename InputIt>
void regex_range<InputIt>::putback(value_type ch) {
  buffer.push(ch);
}

}//namespace lex
#endif// _regex_range_h_
