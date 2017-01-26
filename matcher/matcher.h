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

template <typename CharT>
class matcher {
 public:
  using value_type = CharT;
  using transition_pointer = 
    typename matcher_transition<value_type>::pointer;

  // The default matcher matches an empty string.
  matcher() 
    : transition_ {matcher_transition<value_type>{}.clone()},
      state_ {transition_->initialize()} {}

  // Most matcher objects are constructed from a transition_pointer object.
  explicit matcher(transition_pointer&& f) 
    : transition_ {std::move(f)},
      state_ {transition_->initialize()} {}

  matcher(const matcher& r);
  matcher(matcher&& r);
  matcher& operator=(const matcher& r);
  matcher& operator=(matcher&& r);

  match_state state() const {return state_;}

  // This method takes a character and uses it to update the matcher's state.
  void update(const value_type& ch);

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
template <typename CharT>
matcher<CharT>::matcher(const matcher& r)
  : transition_ {r.transition_->clone()},
    state_ {r.state_} {}
// Move constructor.
template <typename CharT>
matcher<CharT>::matcher(matcher&& r)
  : transition_ {std::move(r.transition_)},
    state_ {r.state_} {}
// Copy assignment.
template <typename CharT>
matcher<CharT>& matcher<CharT>::operator=(const matcher& r) {
  transition_ = r.transition_->clone();
  state_ = r.state_;
  return *this;
}
// Move assignment.
template <typename CharT>
matcher<CharT>& matcher<CharT>::operator=(matcher&& r) {
  transition_ = std::move(r.transition_);
  state_ = r.state_;
  return *this;
}
// The update method checks the state and defers to the transition_pointer
// object if any work is necessary.
template <typename CharT>
void matcher<CharT>::update(const value_type& ch) {
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
  using value_type = typename Transition::value_type;
  using matcher_type = matcher<value_type>;

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
