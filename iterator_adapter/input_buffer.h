/*
 * An input_buffer is designed to mediate the adaptation of an input iterator
 * such as std::istream_iterator<char>(std::cin) into a forward iterator
 * (buffer_iterator). The input_buffer reads just enough values from the
 * input_iterator for the forward iterator properties to work.
 */

#ifndef _input_buffer_h_ 
#define _input_buffer_h_ 

#include "buffer_iterator.h"

#include <iterator>
#include <deque>

namespace lex {
template <typename InputIter, typename Storage = std::deque<
    typename std::iterator_traits<InputIter>::value_type>>
class input_buffer {
 public:
  using value_type = typename std::iterator_traits<InputIter>::value_type;
  using reference = value_type&;
  using storage_type = Storage;
  using size_type = typename storage_type::size_type;
  using difference_type = typename storage_type::difference_type;
  using iterator = buffer_iterator<input_buffer>;
 
  input_buffer(InputIter b, InputIter e) 
    : lag {false},
      current {b}, 
      end_ {e} {}

  size_type size() const {return storage.size();}
  bool empty() const {return storage.empty();}

  iterator begin();
  iterator end();

  // get() attempts to reads in the next value from current. If successful,
  // the value will be appended to the storage with true as a return value.
  // If not, then current == end and the value false will be returned.
  bool get();

  // These are storage accessors.
  value_type operator[](size_type i) const {return storage[i];}
  reference operator[](size_type i) {return storage[i];}

  // This will invalidate all iterators not equal to end().
  iterator flush(iterator it);
 private:
  bool lag; // true if current points to something already read.
  InputIter current;
  InputIter end_;
  storage_type storage;
};

template <typename InputIter, typename Storage>
typename input_storage<InputIter, Storage>::iterator
input_storage<InputIter, Storage>::begin() {
  return iterator(this, 0);
}

template <typename InputIter, typename Storage>
typename input_storage<InputIter, Storage>::iterator
input_storage<InputIter, Storage>::end() {
  return iterator(this);
}

// We use the lag flag because we do not want to increment the input iterator
// current until we actually need to dereference it.
template <typename InputIter, typename Storage>
bool input_storage<InputIter, Storage>::get() {
  if (lag) {
    ++current;
  }
  if (current == end_) return false;
  storage.push_back(*current);
  lag = true;
  return true;
}

// This will invalidate iterators.
template <typename InputIter, typename Storage>
typename input_storage<InputIter, Storage>::iterator
input_storage<InputIter, Storage>::flush(iterator it) {
  auto index = it.index();
  if (index > size()) {
    it.update_storage();
  }
  storage.erase(storage.begin(), storage.begin() + index);
  return begin();
}

}//namespace lex
#endif// _input_buffer_h_ 
