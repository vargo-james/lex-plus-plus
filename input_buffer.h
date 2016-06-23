#ifndef _input_buffer_h_ 
#define _input_buffer_h_ 

#include "buffer_iterator.h"

#include <iterator>
#include <deque>

namespace token_iterator {
template <typename InputIter, typename Storage = std::deque<
    typename std::iterator_traits<InputIter>::value_type>>
class input_buffer {
 public:
  using value_type = typename std::iterator_traits<InputIter>::value_type;
  using reference = value_type&;
  using buffer_type = Storage;
  using size_type = typename buffer_type::size_type;
  using difference_type = typename buffer_type::difference_type;
  using iterator = buffer_iterator<input_buffer>;
 
  input_buffer(InputIter b, InputIter e) 
    : lag {false},
      current {b}, 
      end_ {e} {}

  size_type size() const {return buffer.size();}
  bool empty() const {return buffer.empty();}

  iterator begin();
  iterator end();

  void set_lag(size_type i) {lag = i;}
  bool get();

  value_type operator[](size_type i) const {return buffer[i];}
  reference operator[](size_type i) {return buffer[i];}

  iterator flush(iterator it);
 private:
  bool lag;
  InputIter current;
  InputIter end_;
  buffer_type buffer;
};

template <typename InputIter, typename Storage>
typename input_buffer<InputIter, Storage>::iterator
input_buffer<InputIter, Storage>::begin() {
  return iterator(this, 0);
}

template <typename InputIter, typename Storage>
typename input_buffer<InputIter, Storage>::iterator
input_buffer<InputIter, Storage>::end() {
  return iterator(this);
}

template <typename InputIter, typename Storage>
bool input_buffer<InputIter, Storage>::get() {
  if (lag) ++current;
  if (current == end_) return false;
  buffer.push_back(*current);
  lag = true;
  return true;
}

template <typename InputIter, typename Storage>
typename input_buffer<InputIter, Storage>::iterator
input_buffer<InputIter, Storage>::flush(iterator it) {
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
