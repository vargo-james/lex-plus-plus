#ifndef _simple_regex_h_
#define _simple_regex_h_

#include "regex_state_transition.h"
#include "regex_types.h"

#include <utility>

#include <iostream>
namespace token_iterator {

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
class simple_regex {
 public:
  using char_type = Char;
  using state_transition = state_transition_ptr<char_type>;

  // The default regex matches an empty string.
  simple_regex() 
    : transition_ {regex_state_transition<char_type>{}.clone()},
      state_ {transition_->initialize()} {}

  explicit simple_regex(state_transition&& f) 
    : transition_ {std::move(f)},
      state_ {transition_->initialize()} {}

  simple_regex(const simple_regex& r);
  simple_regex& operator=(const simple_regex& r);
  simple_regex(simple_regex&& r);
  simple_regex& operator=(simple_regex&& r);

  regex_state state() const {return state_;}
  void update(const char_type& ch);
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
#endif// _simple_regex_h_
