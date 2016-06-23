// Should the constructor be public??
// Maybe it should be private and instances should only be accessible
// via calls to begin(), copy, operator++(int), ...
#ifndef _buffer_iterator_h_
#define _buffer_iterator_h_

#include <iterator>
#include <limits>
namespace token_iterator {
template <typename InputBuffer>
class buffer_iterator {
 public:
  using buffer_type = InputBuffer;
  using size_type = typename buffer_type::size_type;
  using difference_type = typename buffer_type::difference_type;
  using value_type = typename buffer_type::value_type;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::forward_iterator_tag;

  buffer_iterator(buffer_type* buffer, size_type i);
  buffer_iterator(buffer_type* buffer);

  buffer_iterator(const buffer_iterator&) = default;
  buffer_iterator& operator= (const buffer_iterator&) = default;
  buffer_iterator(buffer_iterator&&) = default;
  buffer_iterator& operator=(buffer_iterator&&) = default;

  size_type index() const {return index_;}

  reference operator*();
  pointer operator->();

  bool operator==(buffer_iterator other);
  bool operator!=(buffer_iterator other) {return !(*this == other);}

  buffer_iterator& operator++();
  buffer_iterator operator++(int);

  void update_buffer();

  bool is_updated() const {
    return index_ < buffer_ptr->size() ||
           index_ == npos(); 
  }

 private:
  buffer_type* buffer_ptr;
  size_type index_;

  static size_type npos() {
    return std::numeric_limits<size_type>::max();
  }
};

template <typename InputBuffer>
buffer_iterator<InputBuffer>::buffer_iterator(buffer_type* buffer, 
    size_type i)
  : buffer_ptr {buffer},
    index_ {i} {}

template <typename InputBuffer>
buffer_iterator<InputBuffer>::buffer_iterator(buffer_type* buffer)
  : buffer_iterator(buffer, npos()) {}

template <typename InputBuffer>
typename buffer_iterator<InputBuffer>::reference 
buffer_iterator<InputBuffer>::operator*() {
  update_buffer();
  return buffer_ptr->operator[](index_);
}

template <typename InputBuffer>
typename buffer_iterator<InputBuffer>::pointer
buffer_iterator<InputBuffer>::operator->() {
  update_buffer();
  return &(buffer_ptr->operator[](index_));
}

template <typename InputBuffer>
bool buffer_iterator<InputBuffer>::operator==(buffer_iterator other) {
  if (buffer_ptr != other.buffer_ptr) return false;

  if (index_ == other.index_) return true;

  if (index_ > other.index_) {
    return other == *this;
  }

  if (is_updated()) return false;
  update_buffer();
  return *this == other;
}

template <typename InputBuffer>
buffer_iterator<InputBuffer>& 
buffer_iterator<InputBuffer>::operator++() {
  if (index_ == npos()) return *this;
  ++index_;
  return *this;
}

template <typename InputBuffer>
buffer_iterator<InputBuffer> 
buffer_iterator<InputBuffer>::operator++(int) {
  auto ind = index_;
  this->operator++();
  return buffer_iterator(buffer_ptr, ind);
}


template <typename InputBuffer>
void  buffer_iterator<InputBuffer>::update_buffer() {
  
  if (index_ == npos()) return;
  while (index_ >= buffer_ptr->size()) {
    if (!buffer_ptr->get()) {
      index_ = npos();
      return;
    }
  }
}
}//namespace token_iterator
#endif// _buffer_iterator_h_
