#ifndef _simple_queue_h_
#define _simple_queue_h_

#include "optional.h"

#include <array>
#include <utility>

namespace lex {

template <typename T, typename Integer, Integer Size>
class simple_queue {
 public:
  using value_type = T;
  using size_type = Integer;

  simple_queue() {}

  bool empty() const {return head == tail;}
  bool full() const {return (tail + 1) % (Size + 1) == head;}
  Integer capacity() const {return Size;}


  bool enqueue(const T& value);
  bool enqueue(T&& value);
  optional<T> get();

 private:
  std::array<T,Size + 1> buffer;
  Integer head {0};
  Integer tail {0};

  Integer inc(Integer& index) {return (index++) % (Size + 1);}
};

template <typename T, typename Integer, Integer Size>
optional<T> simple_queue<T,Integer,Size>::get() {
  if (empty()) {
    return {};
  }
  return buffer[inc(head)];
}

template <typename T, typename Integer, Integer Size>
bool simple_queue<T,Integer,Size>::enqueue(const T& value) {
  if (full()) return false;
  buffer[inc(tail)] = value;
  return true;
}

template <typename T, typename Integer, Integer Size>
bool simple_queue<T,Integer,Size>::enqueue(T&& value) {
  if (full()) return false;
  buffer[inc(tail)] = std::move(value);
  return true;
}

}//namespace lex
#endif// _simple_queue_h_
