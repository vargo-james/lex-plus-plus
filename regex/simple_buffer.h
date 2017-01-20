#ifndef _simple_buffer_h_
#define _simple_buffer_h_

namespace lex {
template <typename T>
class simple_buffer {
 public:
  simple_buffer(): full {false} {}
  explicit simple_buffer(const T& v): val_ {v}, full {true} {}

  simple_buffer(const simple_buffer&) = default;
  simple_buffer(simple_buffer&&) = default;

  simple_buffer& operator=(const simple_buffer&) = default;
  simple_buffer& operator=(simple_buffer&&) = default;

  bool empty() const {return !full;}
  bool set(const T& val);
  bool get(T& out);
  T peek() const {return val_;}

  void clear() {full = false;}
 private:
  T val_;
  bool full;
};

template <typename T>
bool simple_buffer<T>::set(const T& val) {
  if (full) {
    return false;
  }

  val_ = val;
  return full = true;
}

template <typename T>
bool simple_buffer<T>::get(T& out) {
  if (!full) {
    return false;
  }
  out = val_;
  full = false;
  return true;
}
}//namespace lex
#endif// _simple_buffer_h_
