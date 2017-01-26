/*
 * This buffer acts like a stack.
 */
#ifndef _simple_buffer_h_
#define _simple_buffer_h_

#include <array>

namespace lex {
template <typename T, unsigned N = 1>
class simple_buffer {
 public:
  simple_buffer(): index {0} {}

  simple_buffer(const simple_buffer&) = default;
  simple_buffer(simple_buffer&&) = default;

  simple_buffer& operator=(const simple_buffer&) = default;
  simple_buffer& operator=(simple_buffer&&) = default;

  bool empty() const;
  bool push(const T& val);
  bool get(T& out);

  void clear();
 private:
  std::array<T,N> buffer;
  unsigned char index;
};

template <typename T, unsigned N>
void simple_buffer<T,N>::clear() {
  index = 0;
}
template <typename T, unsigned N>
bool simple_buffer<T,N>::empty() const {
  return index == 0;
}
template <typename T, unsigned N>
bool simple_buffer<T,N>::push(const T& val) {
  if (index >= N) return false;
  buffer[index++] = val;
  return true;
}
template <typename T, unsigned N>
bool simple_buffer<T,N>::get(T& out) {
  if (index) {
    out = buffer[--index];
    return true;
  }
  return false;
}


}//namespace lex
#endif// _simple_buffer_h_
