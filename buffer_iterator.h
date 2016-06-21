// Should the constructor be public??
// Maybe it should be private and instances should only be accessible
// via calls to begin(), copy, operator++(int), ...
#ifndef _buffer_iterator_h_
#define _buffer_iterator_h_

#include <iterator>
#include <limits>

namespace token_iterator {
template <typename InputBuffer>
class BufferIterator {
 public:
  using size_type = typename InputBuffer::size_type;
  using difference_type = typename InputBuffer::difference_type;
  using value_type = typename InputBuffer::value_type;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::forward_iterator_tag;

  BufferIterator(InputBuffer* buffer, size_type i);
  BufferIterator(InputBuffer* buffer);
  BufferIterator(const BufferIterator&) = default;
  BufferIterator& operator= (const BufferIterator&) = default;
  BufferIterator(BufferIterator&&) = default;
  BufferIterator& operator=(BufferIterator&&) = default;

  size_type index() const {return index_;}

  reference operator*();
  pointer operator->();

  bool operator==(BufferIterator other);
  bool operator!=(BufferIterator other) {return !(*this == other);}

  BufferIterator& operator++();
  BufferIterator operator++(int);

  void update_buffer();
 private:
  InputBuffer* input_buffer;
  size_type index_;

  static size_type npos() {
    return std::numeric_limits<size_type>::max();
  }
};

template <typename InputBuffer>
BufferIterator<InputBuffer>::BufferIterator(InputBuffer* buffer, 
    size_type i)
  : input_buffer {buffer},
    index_ {i} {}

template <typename InputBuffer>
BufferIterator<InputBuffer>::BufferIterator(InputBuffer* buffer)
  : BufferIterator(buffer, npos()) {}

template <typename InputBuffer>
typename BufferIterator<InputBuffer>::reference 
BufferIterator<InputBuffer>::operator*() {
  update_buffer();
  return input_buffer->operator[](index_);
}

template <typename InputBuffer>
typename BufferIterator<InputBuffer>::pointer
BufferIterator<InputBuffer>::operator->() {
  update_buffer();
  return &(input_buffer->operator[](index_));
}

template <typename InputBuffer>
bool BufferIterator<InputBuffer>::operator==(BufferIterator other) {
  update_buffer();
  other.update_buffer();
  if (input_buffer == other.input_buffer && index_ == other.index_) {
    return true;
  }
  return false;
}

template <typename InputBuffer>
BufferIterator<InputBuffer>& BufferIterator<InputBuffer>::operator++() {
  if (index_ == npos()) return *this;
  ++index_;
  update_buffer();
  return *this;
}

template <typename InputBuffer>
BufferIterator<InputBuffer> BufferIterator<InputBuffer>::operator++(int) {
  auto ind = index_;
  this->operator++();
  return BufferIterator(input_buffer, ind);
}


template <typename InputBuffer>
void  BufferIterator<InputBuffer>::update_buffer() {
  if (index_ == npos()) return;
  while (index_ >= input_buffer->size()) {
    if (!input_buffer->get()) {
      index_ = npos();
      return;
    }
  }
}
}//namespace token_iterator
#endif// _buffer_iterator_h_
