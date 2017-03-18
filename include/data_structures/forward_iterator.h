#ifndef _forward_iterator_h_
#define _forward_iterator_h_

#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace lex {

template <typename T, typename Ref>
struct proto_iterator {
  using value_type = T;
  using reference = Ref;
  using pointer = typename std::remove_reference<Ref>::type*;

  proto_iterator() = default;
  virtual ~proto_iterator() = default;

  virtual std::unique_ptr<proto_iterator> clone() const = 0; 
  virtual reference operator*() = 0;
  virtual pointer operator->() = 0;
  virtual void operator++() = 0;
  virtual bool 
  operator==(const std::unique_ptr<proto_iterator>& other) const = 0;
};

template <typename ForwardIt>
class iterator_holder : 
  public proto_iterator<typename std::iterator_traits<ForwardIt>::value_type, 
                        typename std::iterator_traits<ForwardIt>::reference> { 
 public:
  using base_type = 
    proto_iterator<typename std::iterator_traits<ForwardIt>::value_type, 
                   typename std::iterator_traits<ForwardIt>::reference>;
  using typename base_type::value_type;
  using typename base_type::reference;
  using typename base_type::pointer;

  iterator_holder() = default;
  iterator_holder(ForwardIt it)
    : iter(std::move(it)) {} 

  iterator_holder(const iterator_holder& other) = default;
  iterator_holder(iterator_holder&& other) = default;

  iterator_holder& operator=(const iterator_holder& other) = default;
  iterator_holder& operator=(iterator_holder&& other) = default;

  std::unique_ptr<base_type> clone() const {
    return std::make_unique<iterator_holder>(*this);
  }

  reference operator*() override {return *iter;}
  pointer operator->() override {return std::addressof(*iter);}
  void operator++() override {++iter;}

  bool operator==(const std::unique_ptr<base_type>& other) const override;
 private:
  ForwardIt iter;
};

template <typename ForwardIt>
bool iterator_holder<ForwardIt>::operator==(
  const std::unique_ptr<base_type>& other) const {
  auto ptr = dynamic_cast<const iterator_holder*>(other.get());
  return (ptr == nullptr)? false : iter == ptr->iter;
}

enum enabler {};
template <typename Condition>
using DisableIf = typename std::enable_if<!Condition::value, 
      enabler>::type;



template <typename T, typename Ref = T&>
class forward_iterator {
 public:
  using value_type = T;
  using reference = Ref;
  using pointer = typename std::remove_reference<Ref>::type*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  forward_iterator()
    : impl(nullptr) {}
  template <typename Iterator> 
  forward_iterator()
    : impl(std::make_unique<iterator_holder<Iterator>>()) {}

  forward_iterator(const forward_iterator<T,Ref>& other)
    : impl(other.impl->clone()) {}
  forward_iterator(forward_iterator&& other)
    : impl(std::move(other.impl)) {}

  template <typename Iterator,
            typename = typename std::enable_if< !
              (std::is_same<
                lex::forward_iterator<T,Ref>, Iterator
              >::value), enabler>::type
            >
  forward_iterator(Iterator iter) 
    : impl(std::make_unique<iterator_holder<Iterator>>(std::move(iter))) {}

  forward_iterator& operator=(const forward_iterator& other) {
    impl = other.impl->clone();
    return *this;
  }
  forward_iterator& operator=(forward_iterator&& other) {
    impl = std::move(other.impl);
    return *this;
  }

  ~forward_iterator() = default;

  forward_iterator& operator++();
  forward_iterator operator++(int);

  reference operator*() {return impl->operator*();}
  pointer operator->() {return impl->operator->();}

  void swap(forward_iterator& other);

  template <typename U, typename R>
  friend bool operator==(const forward_iterator<U,R>& left, 
      const forward_iterator<U,R>& right);
 private:
  std::unique_ptr<proto_iterator<T,Ref>> impl;
};

template <typename T, typename Ref>
forward_iterator<T,Ref>& forward_iterator<T,Ref>::operator++() {
  impl->operator++();
  return *this;
}

template <typename T, typename Ref>
forward_iterator<T,Ref> forward_iterator<T,Ref>::operator++(int) {
  auto copy = *this;
  impl->operator++();
  return copy;
}

template <typename T, typename Ref>
void forward_iterator<T,Ref>::swap(forward_iterator& other) {
  using std::swap;
  swap(impl, other.impl);
}

template <typename T, typename Ref>
void swap(forward_iterator<T,Ref>& left, forward_iterator<T,Ref>& right) {
  left.swap(right);
}

template <typename T, typename R>
bool operator==(const forward_iterator<T,R>& left, 
    const forward_iterator<T,R>& right) {
  return left.impl->operator==(right.impl);
}

template <typename T, typename Ref>
bool operator!=(const forward_iterator<T,Ref>& left, 
    const forward_iterator<T,Ref>& right) {
  return !(left == right);
}

template <typename ForwardIt>
forward_iterator<typename std::iterator_traits<ForwardIt>::value_type,
                 typename std::iterator_traits<ForwardIt>::reference>
make_forward_iterator(ForwardIt it) {
  using Value = typename std::iterator_traits<ForwardIt>::value_type;
  using Reference = typename std::iterator_traits<ForwardIt>::reference;
  return forward_iterator<Value, Reference>(it);
}

}//namespace lex
#endif// _forward_iterator_h_
