#ifndef _matcher_h_
#define _matcher_h_

#include "matcher_state_transition.h"
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
  using state_transition = state_transition_ptr<char_type>;

  // The default matcher matches an empty string.
  matcher() 
    : transition_ {match_state_transition<char_type>{}.clone()},
      state_ {transition_->initialize()} {}

  explicit matcher(state_transition&& f) 
    : transition_ {std::move(f)},
      state_ {transition_->initialize()} {}

  matcher(const matcher& r);
  matcher& operator=(const matcher& r);
  matcher(matcher&& r);
  matcher& operator=(matcher&& r);

  match_state state() const {return state_;}
  void update(const char_type& ch);
  void initialize() {state_ = transition_->initialize();}
 private:
  state_transition transition_;
  match_state state_;
};

template <typename Char>
matcher<Char>::matcher(const matcher& r)
  : transition_ {r.transition_->clone()},
    state_ {r.state_} {}

template <typename Char>
matcher<Char>& matcher<Char>::operator=(const matcher& r) {
  transition_ = r.transition.clone();
  state_ = r.state_;
}

template <typename Char>
matcher<Char>::matcher(matcher&& r)
  : transition_ {std::move(r.transition_)},
    state_ {r.state_} {}

template <typename Char>
matcher<Char>& matcher<Char>::operator=(matcher&& r) {
  transition_ = std::move(r.transition_);
  state_ = r.state_;
  return *this;
}

template <typename Char>
void matcher<Char>::update(const char_type& ch) {
  if (state_ == match_state::MISMATCH) {
    return; 
  }
  if (state_ == match_state::FINAL_MATCH) {
    state_ = match_state::MISMATCH;
    return;
  }

  state_ = transition_->update(ch);
  return;
}

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
