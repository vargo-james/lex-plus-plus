#ifndef _matcher_h_
#define _matcher_h_

#include "matcher/matcher_impl.h"
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

template <typename CharT, typename Traits>
class matcher {
 public:
  using value_type = CharT;
  using traits_type = Traits;
  using impl_pointer = 
    typename matcher_impl<CharT, Traits>::pointer;

  // The default matcher matches an empty string.
  matcher() 
    : impl_ {matcher_impl<CharT, Traits>{}.clone()},
      state_ {impl_->initialize()} {}

  // Most matcher objects are constructed from a impl_pointer object.
  explicit matcher(impl_pointer&& f) 
    : impl_ {std::move(f)},
      state_ {impl_->initialize()} {}

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
  // which was constructed from a impl_pointer object.
  void initialize() {state_ = impl_->initialize();}
 private:
  impl_pointer impl_;
  match_state state_;
};

// Copy constructor.
template <typename CharT, typename Traits>
matcher<CharT, Traits>::matcher(const matcher& r)
  : impl_ {r.impl_->clone()},
    state_ {r.state_} {}
// Move constructor.
template <typename CharT, typename Traits>
matcher<CharT, Traits>::matcher(matcher&& r)
  : impl_ {std::move(r.impl_)},
    state_ {r.state_} {}
// Copy assignment.
template <typename CharT, typename Traits>
matcher<CharT, Traits>& matcher<CharT, Traits>::operator=(const matcher& r) {
  impl_ = r.impl_->clone();
  state_ = r.state_;
  return *this;
}
// Move assignment.
template <typename CharT, typename Traits>
matcher<CharT, Traits>& matcher<CharT, Traits>::operator=(matcher&& r) {
  impl_ = std::move(r.impl_);
  state_ = r.state_;
  return *this;
}
// The update method checks the state and defers to the impl_pointer
// object if any work is necessary.
template <typename CharT, typename Traits>
void matcher<CharT, Traits>::update(const value_type& ch) {
  if (state_ == match_state::MISMATCH) {
    return; 
  }
  else if (state_ == match_state::FINAL_MATCH) {
    state_ = match_state::MISMATCH;
    return;
  }
  else {  // state == match_state::UNDECIDED or match_state::MATCH
    state_ = impl_->update(ch);
    return;
  }
}

// All nontrivial matcher objects are created from a impl_pointer object.
// So we use this factory class to facilitate the creation of a matcher object
// directly from the constructor arguments of the impl_pointer object.
template <typename MatcherImpl>
struct matcher_factory {
  using value_type = typename MatcherImpl::value_type;
  using traits_type = typename MatcherImpl::traits_type;
  using matcher_type = matcher<value_type, traits_type>;

  template <typename ...Args>
  matcher_type create(Args&& ...args) const {
    using std::make_unique;
    using std::forward;
    return 
      matcher_type(make_unique<MatcherImpl>(forward<Args>(args)...));
  }
};


}//namespace lex
#endif// _matcher_h_
