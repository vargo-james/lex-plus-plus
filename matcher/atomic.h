#ifndef _atomic_h_
#define _atomic_h_

#include "matcher/matcher.h"

#include <vector>

namespace lex {

// The Regex transition object corresponding to a single char.
namespace detail {
template <typename CharT>
struct singleton_matcher_transition 
  : public matcher_transition_cloner<
           singleton_matcher_transition<CharT>, CharT
         > {
 public:
  using typename matcher_transition<CharT>::value_type;

  singleton_matcher_transition(const value_type& t)
    : match {t} {}

  // This only matches when it was previously UNDECIDED and the 
  // character it receives (t), is a match.
  match_state update(const value_type& t) override;
  match_state initialize() override;
 private:
  value_type match;
};

template <typename CharT>
match_state singleton_matcher_transition<CharT>::update(const value_type& t) {
  if (t == match) {
    return match_state::FINAL_MATCH;
  }
  return match_state::MISMATCH;
}

template <typename CharT>
match_state singleton_matcher_transition<CharT>::initialize() {
  return match_state::UNDECIDED;
}

}//namespace detail
template <typename CharT>
matcher<CharT> singleton_matcher(const CharT& ch) {
  matcher_factory<detail::singleton_matcher_transition<CharT>> fac;
  return fac.create(ch);
}


// The matcher object corresponding to a predicate on a single char. 
// E.g. [:alpha:]
// In order for the initialize() method to be semantically correct, 
// the predicate Pred must be stateless.
namespace detail {
template <typename CharT>
class predicate_matcher_transition 
  : public matcher_transition_cloner<
             predicate_matcher_transition<CharT>, CharT
           > {
 public:
  using typename matcher_transition<CharT>::value_type;
  using predicate_type = predicate_type_t<CharT>;

  predicate_matcher_transition(const predicate_type& p) 
    : pred {p} {}

  match_state update(const CharT& t) override;
  match_state initialize() override;
 private:
  predicate_type pred;
};

template <typename CharT>
match_state 
predicate_matcher_transition<CharT>::update(const CharT& t) {
  if (pred(t)) {
    return match_state::FINAL_MATCH;
  }
  return match_state::MISMATCH;
}

template <typename CharT>
match_state predicate_matcher_transition<CharT>::initialize() {
  return match_state::UNDECIDED;
}
}//namespace detail
template <typename CharT>
matcher<CharT> predicate_matcher(const predicate_type_t<CharT>& p) {
  matcher_factory<detail::predicate_matcher_transition<CharT>> fac;
  return fac.create(p);
}

// This creates a matcher that matches a '.'. The predicate
// always returns true.
template <typename CharT>
matcher<CharT> universal_singleton_matcher() {
  return predicate_matcher<CharT>([](const auto&){return true;});
}


}//namespace lex
#endif// _atomic_h_
