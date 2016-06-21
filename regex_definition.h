#ifndef _regex_definition_h_
#define _regex_definition_h_

#include <functional>
#include <memory>
#include <utility>

#include <iostream>
namespace token_iterator {
enum class regex_state {MATCH, MISMATCH, UNDECIDED};

regex_state flip_state(regex_state s) {
  switch (s) {
  case regex_state::MATCH:
    return regex_state::MISMATCH;
  case regex_state::MISMATCH:
    return regex_state::MATCH;
  default:
    break;
  }
  return regex_state::UNDECIDED;
}

template <typename RegexIterator>
using regex_type = 
  typename std::iterator_traits<RegexIterator>::value_type;

template <typename RegexIterator>
using char_type = typename regex_type<RegexIterator>::char_type;

//DEBUG
char rep(regex_state s) {
  switch (s) {
  case regex_state::MATCH:
    return 'M';
  case regex_state::MISMATCH:
    return 'X';
  case regex_state::UNDECIDED:
    return 'U';
  }
  return '0';
}

template <typename Char>
class regex_state_transition {
 public:
  using char_type = Char;
  using pointer = std::unique_ptr<regex_state_transition>;

  regex_state_transition() = default;
  ~regex_state_transition() = default;

  virtual regex_state 
  update(const char_type& ch) {return regex_state::MISMATCH;}
  virtual regex_state 
  initialize() {return regex_state::MATCH;}
  virtual pointer
  clone() const {return std::make_unique<regex_state_transition>();}
};

template <typename Derived, typename Char>
class regex_state_transition_CRTP : public regex_state_transition<Char> {
 public:
  using typename regex_state_transition<Char>::pointer;
  pointer clone() const override {
    return std::make_unique<Derived>(static_cast<Derived const&>(*this));
  }
};

template <typename Char>
class simple_regex {
 public:
  using char_type = Char;
  using state_transition = 
    std::unique_ptr<regex_state_transition<char_type>>; 

  // The default regex matches an empty string.
  simple_regex() 
    : transition_ {std::make_unique<regex_state_transition<char_type>>()},
      state_ {transition_->initialize()} {}

  explicit simple_regex(state_transition&& f) 
    : transition_ {std::move(f)},
      state_ {transition_->initialize()} {}

  simple_regex(state_transition&& f, regex_state s)
    : transition_ {std::move(f)},
      state_ {s} {
    transition_->initialize();
  }

  simple_regex(const simple_regex& r);
  simple_regex& operator=(const simple_regex& r);
  simple_regex(simple_regex&& r);
  simple_regex& operator=(simple_regex&& r);

  regex_state state() const {return state_;}
  void set_state(regex_state s) {state_ = s;}

  void update(const char_type& ch);

  void initialize(regex_state s);
  void initialize() {state_ = transition_->initialize();}
 private:
  state_transition transition_;
  regex_state state_;
};

template <typename Char>
simple_regex<Char>::simple_regex(const simple_regex& r)
  : transition_ {r.transition_->clone()},
    state_ {r.state_} {}

template <typename Char>
simple_regex<Char>& simple_regex<Char>::operator=(const simple_regex& r) {
  transition_ = r.transition.clone();
  state_ = r.state_;
}

template <typename Char>
simple_regex<Char>::simple_regex(simple_regex&& r)
  : transition_ {std::move(r.transition_)},
    state_ {r.state_} {}

template <typename Char>
simple_regex<Char>& simple_regex<Char>::operator=(simple_regex&& r) {
  transition_ = std::move(r.transition_);
  state_ = r.state_;
  return *this;
}

template <typename Char>
void simple_regex<Char>::initialize(regex_state s) {
  transition_->initialize();
  state_ = s;
}

template <typename Char>
void simple_regex<Char>::update(const char_type& ch) {
  if (state_ == regex_state::MISMATCH) {
    return;
  }
  state_ = transition_->update(ch);
  std::cout << rep(state_) << '\n';
  return;
}

using regex = simple_regex<char>;

}//namespace token_iterator
#endif// _regex_definition_h_
