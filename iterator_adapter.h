#ifndef _iterator_adapter_h_
#define _iterator_adapter_h_

#include "input_buffer.h"
#include "buffer_iterator.h"
#include "translator.h"

#include <cstddef>
#include <iterator>
#include <string>

namespace token_iterator {

template <typename InputIter, typename T>
class iterator_adapter {
 public:
  using iterator_category = std::input_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using reference = value_type&;

  using char_type = std::iterator_traits<InputIter>::value_type;
  using string_type = std::basic_string<char_type>;
  using constructor = std::function<void(const string_type&)>;
  using lexer_type = lexer<InputIter,constructor>;

  iterator_adapter(InputIter begin, InputIter end, 
      const translator& tab); 

  iterator_adapter(const iterator_adapter&) = default;
  iterator_adapter& operator=(const iterator_adapter&) = default;
  iterator_adapter(iterator_adapter&&) = default;
  iterator_adapter& operator=(iterator_adapter&&) = default;

  bool operator==(const iterator_adapter& other);
  bool operator!=(const iterator_adapter& other);

  reference operator*() {return current;}
  pointer operator->() {return &current;}

  iterator_adapter& operator++() {get();}
  iterator_adapter operator++(int);

 private:
  lexer_type lex;
  value_type current;

  void get();
};

template <typename InputIter, typename T>
iterator_adapter<InputIter,T>::iterator_adapter(
    InputIter begin, InputIter end, const translator& tab) 
  : buffer {std::make_shared<input_buffer>(begin, end)},
    patterns {&tab} {
  get();
}

template <typename InputIter, typename T>
bool 
iterator_adapter<InputIter,T>::operator==(const iterator_adapter& other) {
  return buffer   == other.buffer &&
         patterns == oterh.patterns &&
         current  == other.current;
}
         
template <typename InputIter, typename T>
bool 
iterator_adapter<InputIter,T>::operator!=(const iterator_adapter& other) {
  return *this != other;
}

template <typename InputIter, typename T>
iterator_adapter<InputIter,T> 
iterator_adapter<InputIter,T>::operator++(int) {
  auto copy = *this;
  get();
  return copy;
}

template <typename InputIter, typename T>
void iterator_adapter<InputIter,T>::get() {
  // The regex match will be the range [begin, accepted).
  auto begin = buffer->begin();
  auto accepted = begin;
  for (auto& pattern : patterns) {
    auto regex = pattern.first;
    // An initialized regex should not be in the MISMATCH state.
    assert(regex.state() != regex_state::MISMATCH);
    for (auto seek = begin; seek != buffer->end();) {
      regex.update(*seek);
      if (regex.state() == regex_state::MISMATCH) {
        break;
      }
      ++seek;
      if (regex.state() == regex_state::MATCH) {
        // [begin, accepted) is a match.
        accepted = seek;
      } 
    }
    // The regex needs to be reinitialized to be used again later.
    regex.initialize();
    // If this regex matches some input, then we are done.
    if (accepted != begin) {
      current = pattern.second(begin, accepted);
      // We flush the processed input from the buffer.
      buffer->flush(accepted);
      return current;
    }
  }
}



}//namespace token_iterator
#endif// _iterator_adapter_h_
