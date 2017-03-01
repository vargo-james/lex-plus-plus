#ifndef _optional_h_
#define _optional_h_

#include <utility>

namespace lex {

template <typename T>
class optional {
 public:
  using value_type = T;

  optional(): valid {false} {}
  optional(const optional& other)
    : val(other.valid? other.val : T{})
    , valid {other.valid} {}

  optional(optional&& other)
    : val(other.valid? std::move(other.val) : T{})
    , valid {other.valid} {}
  optional(const T& value): val(value) , valid {true} {}
  optional(T&& value): val(std::move(value)), valid {true} {}
  optional& operator=(const optional& other);
  optional& operator=(optional&& other);
  optional& operator=(const T& value);
  optional& operator=(T&& value);

  const T* operator->() const {return &val;}
  T* operator->() {return &val;}
  const T& operator*() const {return val;}
  T& operator*() {return val;}

  operator bool() const {return valid;}
  bool has_value() const {return valid;}

  T value() {return val;}
  const T& value() const {return val;}

  template <typename U>
  T value_or(U&& u) const {
    return has_value()? val : static_cast<T>(std::forward<U>(u));
  }
 private:
  T val;
  bool valid;
};

template <typename T>
optional<T>& optional<T>::operator=(const optional& other) {
  val = other.valid? other.val : T{};
  valid = other.valid;
  return *this;
}

template <typename T>
optional<T>& optional<T>::operator=(optional&& other) {
  val = other.valid? std::move(other.val) : T{};
  valid = other.valid;
  return *this;
}

template <typename T>
optional<T>& optional<T>::operator=(const T& value) {
  val = value;
  valid = true;
  return *this;
}

template <typename T>
optional<T>& optional<T>::operator=(T&& value) {
  val = std::move(value);
  valid = true;
  return *this;
}


}//namespace lex
#endif// _optional_h_
