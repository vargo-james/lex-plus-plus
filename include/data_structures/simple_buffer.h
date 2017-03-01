/*
 * This buffer acts like a stack.
 *
 * Possible functionality:
 *   push(optional<T> t) {
 *     if (t) push(t);
 *   }
 */
#ifndef _simple_buffer_h_
#define _simple_buffer_h_

#include "optional.h"

#include <array>
#include <utility>

namespace lex {
template <typename T, typename Integer = unsigned char, Integer N = 1>
class simple_buffer {
 public:
  using value_type = T;
  using size_type = Integer;

  simple_buffer(): index {0} {}

  simple_buffer(const simple_buffer&) = default;
  simple_buffer(simple_buffer&&) = default;

  simple_buffer& operator=(const simple_buffer&) = default;
  simple_buffer& operator=(simple_buffer&&) = default;

  bool empty() const {return index == 0;}
  bool full() const {return index == buffer.size();}
  size_type capacity() const {return N;}

  optional<T> get();
  bool push(const T& val);
  bool push(T&& val);

  void clear() {index = 0;}
 private:
  std::array<T,N> buffer;
  Integer index;
};

template <typename T, typename Integer, Integer N>
bool simple_buffer<T,Integer, N>::push(const T& val) {
  if (full()) return false;
  buffer[index++] = val;
  return true;
}

template <typename T, typename Integer, Integer N>
bool simple_buffer<T,Integer,N>::push(T&& val) {
  if (full()) return false;
  buffer[index++] = std::move(val);
  return true;
}

template <typename T, typename Integer, Integer N>
optional<T> simple_buffer<T,Integer,N>::get() {
  if (index) {
    return buffer[--index];
  }
  else {
    return {};
  }
}

template <typename Source, 
          typename Buffer = simple_buffer<typename Source::value_type>>
class buffered : public Source {
 public:
  using Source::Source;
  using typename Source::value_type;
  buffered(const buffered& other): Source(other) {}
  buffered(buffered&& other): Source(std::move(other)) {}

  optional<value_type> get();
  bool putback(const value_type& val) {return buffer.push(val);}
  bool putback(value_type&& val) {return buffer.push(std::move(val));}
  bool empty() const {return buffer.empty() && Source::empty();}
 private:
  Buffer buffer;
};

template <typename Source, typename Buffer>
optional<typename Source::value_type>
buffered<Source,Buffer>::get() {
  auto ret = buffer.get();
  return ret? ret : Source::get();
}

}//namespace lex
#endif// _simple_buffer_h_
