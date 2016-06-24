#ifndef _iterator_adapter_h_
#define _iterator_adapter_h_

#include "input_buffer.h"
#include "buffer_iterator.h"
#include "translator.h"
#include "lexer_impl.h"

#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <string>

namespace lex {

// T must be default constructible.
template <typename InputIter, typename T>
class iterator_adapter {
 public:
  using iterator_category = std::input_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using reference = value_type&;

  using buffer_type = input_buffer<InputIter>;
  using char_type = typename std::iterator_traits<InputIter>::value_type;
  using string_type = std::basic_string<char_type>;
  using function_type = std::function<value_type(const string_type&)>;

  iterator_adapter(InputIter begin, InputIter end, 
      translator<iterator_adapter>* table); 
  iterator_adapter() 
    : buffer_p {nullptr},
      trans_p {nullptr},
      lex_impl(nullptr, nullptr),
      index {eof()},
      lag {false} {}


  iterator_adapter(const iterator_adapter&) = default;
  iterator_adapter& operator=(const iterator_adapter&) = default;
  iterator_adapter(iterator_adapter&&) = default;
  iterator_adapter& operator=(iterator_adapter&&) = default;

  bool operator==(iterator_adapter& other);
  bool operator!=(iterator_adapter& other);

  reference operator*();
  pointer operator->();

  iterator_adapter& operator++();
  iterator_adapter operator++(int);
 private:
  std::shared_ptr<buffer_type> buffer_p;
  translator<iterator_adapter>* trans_p;
  lexer_impl<iterator_adapter> lex_impl;

  size_t index;
  bool lag;
  value_type current;

  bool lex();
  void update();
  static size_t eof() {return std::numeric_limits<size_t>::max();}
  bool done() const {return index == eof();}
};

template <typename InputIter, typename T>
iterator_adapter<InputIter,T>::iterator_adapter(
    InputIter begin, InputIter end, translator<iterator_adapter>* ptr) 
  : buffer_p {std::make_shared<buffer_type>(begin, end)},
    trans_p {ptr},
    lex_impl(buffer_p.get(), trans_p),
    index {0},
    lag {true} {} 

template <typename InputIter, typename T>
bool 
iterator_adapter<InputIter,T>::operator==(iterator_adapter& other) {
  if (index > other.index) return other == *this;

  if (other.done()) {
    update();
    return done();
  }

  if (buffer_p != other.buffer_p) return false;
  // They point to the same buffer.
  if (index == other.index) return true;
  // index < other.index. They point to the same buffer.
  // If other is not lagged, then neither one can point to the end and
  // their indexes are presently accurate.
  // Even if other is lagged, then it can only be one unit past something
  // that was not the end. Therefore *this cannot be pointing to the end.
  // Therefore its index is presently accurate. Therefore, these cannot be
  // equal.
  return false;
}

template <typename InputIter, typename T>
bool 
iterator_adapter<InputIter,T>::operator!=(iterator_adapter& other) {
  return !(*this == other);
}

template <typename InputIter, typename T>
iterator_adapter<InputIter,T>&
iterator_adapter<InputIter,T>::operator++() {
  update();
  if (!done()) {
    ++index;
    lag = true;
  }
  return *this;
}

template <typename InputIter, typename T>
iterator_adapter<InputIter,T> 
iterator_adapter<InputIter,T>::operator++(int) {
  update();
  auto copy = *this;
  this->operator++();
  return copy;
}

template <typename InputIter, typename T>
bool iterator_adapter<InputIter,T>::lex() {
  auto result = lex_impl.do_lex();
  auto trans_it = result.second;
  // We check for a failed read.
  if (trans_it == trans_p->end()) {
    return false;
  }

  // We form a string from the matching input.
  auto buff_it = result.first;
  auto string_result = string_type(buffer_p->begin(), buff_it);
  // Now we call the function corresponding to the matched regex.
  current = (trans_it->second)(string_result);
  // We flush the processed input from the buffer.
  buffer_p->flush(buff_it);
  return true;
}

template <typename InputIter, typename T>
void iterator_adapter<InputIter,T>::update() {
  if (done()) {
    return;
  }
  if (lag) {
    auto success = lex();
    if (!success) {
      index = eof();
    }
    lag = false;
  }
}

template <typename InputIter, typename T>
typename iterator_adapter<InputIter,T>::reference 
iterator_adapter<InputIter,T>::operator*() {
  update();
  return current;
}
template <typename InputIter, typename T>
typename iterator_adapter<InputIter,T>::pointer 
iterator_adapter<InputIter,T>::operator->() {
  update();
  return &current;
}


}//namespace lex
#endif// _iterator_adapter_h_

