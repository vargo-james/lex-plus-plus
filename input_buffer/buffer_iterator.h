// NOTES:
//  update_buffer() can probably be private.
//
// Should the constructor be public??
// Maybe it should be private and instances should only be accessible
// via calls to begin(), copy, operator++(int), ...



/*
 * A buffer_iterator is a forward_iterator for the input_buffer class.
 * Essentially, this serves the purpose of taking an input iterator and
 * adapting into a forward iterator. The InputBuffer class that is used
 * in the template is used as a storage buffer to make that work.
 * For example, using this class, we can take a pair of iterators such as
 * std::istream_iterator<char>(std::cin) and std::istream_iterator<char>{}
 * and then adapt them into a pair of forward iterators. 
 *
 * Of course an intermediate buffer is necessary. And it would be trivial
 * to simply read in all the input iterator values into a vector and then
 * take the forward iterators already defined for that vector. However,
 * this class has the useful property that it only reads values from 
 * the input iterator when it absolutely has to. This is useful for
 * the istream_iterators, for example, because we do not want our processing
 * of the forward iterators to hang while we wait for an EOF from the user.
 * This class will allow all processing of the iterators to occur and will
 * only wait on values that the processing asks for.
 *
 * The implementation here uses a pointer to the buffer and an index into
 * the buffer's storage. This allows us to do increment operations 
 * without making any readings from the input iterator. We only call the
 * input buffers get() function (reading from the input iterator), when
 * we need to dereference an iterator and its corresponding index points
 * past the end of the buffer's current storage. 
 *
 * Reading from the input_iterator is also sometimes necessary 
 * to compare two buffer_iterators. The reason 
 * is that if an index is pointing past the end of the buffer's current
 * storage, that might mean the iterator is already at the end. However, 
 * we cannot know that if we have not read enough from the input. 
 * We cannot know, for example, whether the 5th character a user enters
 * will be past the end of the input until we have attempted to read 5 
 * characters.
 *
 * Thus dereferencing and comparison operations can cause the processing
 * of a buffer_iterator to hang while the program waits for input.
 */

#ifndef _buffer_iterator_h_
#define _buffer_iterator_h_

#include <iterator>
#include <limits>
namespace lex {
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
  // This constructs the the iterator that points to the end
  // of the buffer. The index is allowed to be invalid with respect
  // to the input buffers current storage. An attempt to dereference the 
  // pointer will call the input_buffers get() method until the index
  // is valid. At that point, the value at that point of storage will
  // be returned.
  buffer_iterator(buffer_type* buffer);

  // Copy and move constructors are the default.
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

  // An up to date index points to a valid position in the buffer or
  // it points to the end position.
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
  // We return the address of the appropriate entry in the input_buffer's
  // storage.
  return &(buffer_ptr->operator[](index_));
}

// Iterators must be equality comparable. This test is done to ensure
// that a comparison of the time iter == end() is done with the fewest
// possible reads into the buffer that iter refers to.
template <typename InputBuffer>
bool buffer_iterator<InputBuffer>::operator==(buffer_iterator other) {
  // Clearly the iterators must refer to the same buffer to be equal.
  if (buffer_ptr != other.buffer_ptr) return false;
  // Bitwise equality implies equality.
  if (index_ == other.index_) return true;

  // This recursive call allows us to assume that index_ < other.index_.
  if (index_ > other.index_) {
    return other == *this;
  }
  // Now we may assume index_ < other.index_.
  if (is_updated()) {
    // index_ is accurate, so we may now conclude they are not equal.
    return false;
  }
  // At this point it is absolutely necessary to update the buffer in
  // order to know whether the iterators are equal.
  update_buffer();
  // This recursive call cannot reach the update_buffer() call.
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

// In the case that index points past the end of the buffer, this function
// will attempt to populate the input buffer with input values until the
// index becomes valid. If the input iterator supplying the buffer runs
// out of values first, then the buffers get() method will return false
// and we will set index to npos().
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
}//namespace lex
#endif// _buffer_iterator_h_
