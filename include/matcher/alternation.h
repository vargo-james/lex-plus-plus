/*
 * This file implements the alternation operator on matchers.
 * This is the '|' operation for regexes.
 */

#ifndef _alternation_h_
#define _alternation_h_

#include "matcher/matcher.h"

#include <algorithm>
#include <list>
#include <vector>
#include <utility>

namespace lex {
namespace detail {

template <typename MatcherIterator>
match_state alternation_state(MatcherIterator begin, 
    MatcherIterator end) {
  auto state = match_state::MISMATCH;

  bool undecided;
  for (auto it = begin; it != end; ++it) {
    if (it->state() == match_state::MATCH) {
      return match_state::MATCH;
    }
    if (it->state() == match_state::FINAL_MATCH) {
      state = match_state::FINAL_MATCH;
    }
    if (it->state() == match_state::UNDECIDED) {
      undecided = true;
    }
  }
  
  if (undecided && state == match_state::FINAL_MATCH) {
    return match_state::MATCH;
  }
  return undecided? match_state::UNDECIDED : state;
}

// This defines the transition function for the alternation of several
// regexes.
template <typename Matcher>
class alternation_impl 
  : public matcher_impl_cloner<
             alternation_impl<Matcher>, typename Matcher::value_type, 
             typename Matcher::traits_type
           > {
 public:
  using matcher_type = Matcher;
  using value_type = typename Matcher::value_type;

  alternation_impl(const std::vector<matcher_type>& c)
    : initial_state {c} {}
  alternation_impl(std::vector<matcher_type>&& c)
    : initial_state {std::move(c)} {}

  match_state update(value_type) override;
  match_state initialize() override;
 private:
  std::vector<matcher_type> initial_state;
  std::list<matcher_type> matchers;
};

template <typename Matcher>
match_state alternation_impl<Matcher>::update(value_type ch) {
  // These flags track what we have seen after traversing the list.
  bool undecided {false};
  bool final_match {false};
  bool match {false};

  for (auto it = matchers.begin(); it != matchers.end();) {
    it->update(ch);
    auto state = it->state();
    switch (state) {
    // If one match is found, the matchers is a match. But we still need
    // to update the rest of the matchers.
    case match_state::MATCH:
      match = true;
      ++it;
      break;
    // If a FINAL_MATCH is found this matcher will not need to be looked
    // at again.
    case match_state::FINAL_MATCH:
      final_match = true;
      it = matchers.erase(it);
      break;
    case match_state::UNDECIDED:
      undecided = true;
      ++it;
      break;
    // If a mismatch is found, that matchers will not need to be 
    // considered henceforth, so we may remove it.
    case match_state::MISMATCH:
      it = matchers.erase(it);
      break;
    // This should never happen.
    default:
      ++it;
      break;
    }
  }
  if (match) {
    return match_state::MATCH;
  } else if (final_match && undecided) {
    return match_state::MATCH;
  } else if (final_match) {
    return match_state::FINAL_MATCH;
  } else if (undecided) {
    return match_state::UNDECIDED;
  } else {
    return match_state::MISMATCH;
  }
}

template <typename Matcher>
match_state alternation_impl<Matcher>::initialize() {
  using std::for_each;

  matchers.clear();

  for_each(initial_state.begin(), initial_state.end(),
      [this] (matcher_type& r) {
        if (r.state() != match_state::MISMATCH) {
          this->matchers.push_back(r);
        }
      });

  return alternation_state(initial_state.begin(), initial_state.end());
}
}//namespace detail

/*
typename <CharT>
matcher<CharT> alternation(std::vector<matcher<CharT>>&& matchers) {
  matcher_factory<detail::alternation_impl<matcher<CharT>> fac;
  return fac.create(std::forward<std::vector<matcher<CharT>>>(matchers));
}
*/

template <typename MatcherContainer>
typename MatcherContainer::value_type
alternation(MatcherContainer&& container) {
  using matchers = typename MatcherContainer::value_type;

  matcher_factory<detail::alternation_impl<matchers>> fac;
  return fac.create(std::forward<MatcherContainer>(container));
}

}//namespace lex
#endif// _alternation_h_
