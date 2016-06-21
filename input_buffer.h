#ifndef _input_buffer_h_ 
#define _input_buffer_h_ 

#include "buffer_iterator.h"

#include <iterator>
#include <deque>

namespace token_iterator {
template <typename InputIter, typename Storage = std::deque<
    typename std::iterator_traits<InputIter>::value_type>>
class InputBuffer {
 public:
  using value_type = typename std::iterator_traits<InputIter>::value_type;
  using reference = value_type&;
  using buffer_type = Storage;
  using size_type = typename buffer_type::size_type;
  using difference_type = typename buffer_type::difference_type;
  using iterator = BufferIterator<InputBuffer>;
 
  InputBuffer(InputIter b, InputIter e) 
    : current {b}, 
      end_ {e} {}

  size_type size() const {return buffer.size();}
  bool empty() const {return buffer.empty();}

  iterator begin();
  iterator end();

  bool get();

  value_type operator[](size_type i) const {return buffer[i];}
  reference operator[](size_type i) {return buffer[i];}

  iterator flush(iterator it);
 private:
  InputIter current;
  InputIter end_;
  buffer_type buffer;
};

template <typename InputIter, typename Storage>
typename InputBuffer<InputIter, Storage>::iterator
InputBuffer<InputIter, Storage>::begin() {
  return iterator(this, 0);
}

template <typename InputIter, typename Storage>
typename InputBuffer<InputIter, Storage>::iterator
InputBuffer<InputIter, Storage>::end() {
  return iterator(this);
}

template <typename InputIter, typename Storage>
bool InputBuffer<InputIter, Storage>::get() {
  if (current == end_) return false;
  buffer.push_back(*current++);
  return true;
}

template <typename InputIter, typename Storage>
typename InputBuffer<InputIter, Storage>::iterator
InputBuffer<InputIter, Storage>::flush(iterator it) {
  auto index = it.index();
  it.update_buffer();
  if (it == end()) {
    buffer.clear();
    return begin();
  }
  buffer.erase(buffer.begin(), buffer.begin() + index);
  return begin();
}

}//namespace token_iterator
#endif// _input_buffer_h_ 
