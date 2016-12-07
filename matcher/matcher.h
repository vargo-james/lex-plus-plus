#ifndef _matcher_h_
#define _matcher_h_

#include "matcher/matcher_transition.h"
#include "regex_types.h"

#include <memory>
#include <utility>

namespace lex {

//DEBUG
/*
char rep(match_state s) {
  switch (s) {
  case match_state::MATCH:
    return 'M';
  case match_state::FINAL_MATCH:
    return 'F';
  case match_state::MISMATCH:
    return 'X';
  case match_state::UNDECIDED:
    return 'U';
  }
  return '0';
}
*/

template <typename Char>
class matcher {
 public:
  using char_type = Char;
  using transition_pointer = 
    typename detail::matcher_transition<char_type>::pointer;

  // The default matcher matches an empty string.
  matcher() 
    : transition_ {detail::matcher_transition<char_type>{}.clone()},
      state_ {transition_->initialize()} {}
  // Most matcher objects are constructed from a transition_pointer object.
  explicit matcher(transition_pointer&& f) 
    : transition_ {std::move(f)},
      state_ {transition_->initialize()} {}
  // Copy and Move constructors
  matcher(const matcher& r);
  matcher& operator=(const matcher& r);
  matcher(matcher&& r);
  matcher& operator=(matcher&& r);
  // This method reports the current state.
  match_state state() const {return state_;}
  // This method takes a character and uses it to update the matcher's state.
  void update(const char_type& ch);
  // This method returns the matcher to its original state.
  // If the matcher was copy or move constructed from another matcher, then 
  // the state is converted to the original state of the original matcher 
  // which was constructed from a transition_pointer object.
  void initialize() {state_ = transition_->initialize();}
 private:
  transition_pointer transition_;
  match_state state_;
};

// Copy constructor.
template <typename Char>
matcher<Char>::matcher(const matcher& r)
  : transition_ {r.transition_->clone()},
    state_ {r.state_} {}
// Copy assignment.
template <typename Char>
matcher<Char>& matcher<Char>::operator=(const matcher& r) {
  transition_ = r.transition.clone();
  state_ = r.state_;
}
// Move constructor.
template <typename Char>
matcher<Char>::matcher(matcher&& r)
  : transition_ {std::move(r.transition_)},
    state_ {r.state_} {}
// Move assignment.
template <typename Char>
matcher<Char>& matcher<Char>::operator=(matcher&& r) {
  transition_ = std::move(r.transition_);
  state_ = r.state_;
  return *this;
}
// The update method checks the state and defers to the transition_pointer
// object if any work is necessary.
template <typename Char>
void matcher<Char>::update(const char_type& ch) {
  if (state_ == match_state::MISMATCH) {
    return; 
  }
  else if (state_ == match_state::FINAL_MATCH) {
    state_ = match_state::MISMATCH;
    return;
  }
  else {  // state == match_state::UNDECIDED or match_state::MATCH
    state_ = transition_->update(ch);
    return;
  }
}

// All nontrivial matcher objects are created from a transition_pointer object.
// So we use this factory class to facilitate the creation of a matcher object
// directly from the constructor arguments of the transition_pointer object.
template <typename Transition>
struct matcher_factory {
  using char_type = typename Transition::char_type;
  using matcher_type = matcher<char_type>;

  template <typename ...Args>
  matcher_type create(Args&& ...args) const {
    using std::make_unique;
    using std::forward;
    return 
      matcher_type(make_unique<Transition>(forward<Args>(args)...));
  }
};


}//namespace lex
#endif// _matcher_h_
