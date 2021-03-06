#ifndef _concatenation_h_
#define _concatenation_h_

#include "matcher/matcher.h"

#include <cstddef>
#include <list>
#include <vector>
#include <utility>

namespace lex {
namespace detail {
// We need to handle concatenation of a list of matchers.
// We need to keep a list of all the individual matchers in their
// initial states.
// The current state will be maintained by a list of matchers.
// Each entry in the list represents a parallel progression through
// the concatenation.
template <typename Matcher>
class concatenate_impl 
  : public matcher_impl_cloner<
             concatenate_impl<Matcher>, typename Matcher::value_type,
             typename Matcher::traits_type
           > {
 public:
  using matcher_type = Matcher;
  using value_type = typename matcher_type::value_type;
  using index_type = std::size_t;
  using current_progress = std::pair<matcher_type, index_type>;

  concatenate_impl(const std::vector<matcher_type>& container)
    : initial_state(container) {}
  concatenate_impl(std::vector<matcher_type>&& container)
    : initial_state(std::move(container)) {}

  match_state update(value_type ch) override;
  match_state initialize() override;
 private:
  std::vector<matcher_type> initial_state;
  std::list<current_progress> current;

  void insert_next(typename std::list<current_progress>::const_iterator it);
};

// To understand this implementation consider the concatenation of
// Left = Matcher("[AB]+") with 
// Right = Matcher("[BC]{2}"). 
// i.e. Concatenation(Left, Right) = Matcher("[AB]+[BC]{2}");
// Matchers are spawned in their initial state.
// Steps represent successive char's in the string to match against.
// Matching against "ABACCB":
// Step 0: spawn Left.                state = UNDECIDED
// Step 1 'A': Left (MATCH)        -> spawns Right (considered in step 2)
// Step 2 'B': Left (MATCH)        -> spawns Right (considered in step 3)
//             Right (UNDECIDED)   -> no action
// Step 3 'A': Left (MATCH)        -> spawns Right
//             Right (MISMATCH)    -> detached
//             Right (MISMATCH)    -> detached
// Step 4 'C': Left (MISMATCH)     -> detached
//             Right (UNDECIDED)   -> no action
// Step 5 'C': Right (FINAL_MATCH) -> nothing left to spawn. detached.
//                                    state = FINAL_MATCH.
// Step 6 'B': empty list          -> state = MISMATCH
//
// Step 6 'B': Right (MISMATCH)    -> detached. list is now empty.
//                                    state = MISMATCH
template <typename Matcher>
match_state 
concatenate_impl<Matcher>::update(value_type ch) {
  bool undecided {false};
  bool final_match {false};
  bool match {false};

  for (auto it = current.begin(); it != current.end();) {
    it->first.update(ch);
    auto state = it->first.state();
    // matcher_index points to the next matcher in the initial_state vector.
    auto matcher_index = it->second;
    switch (state) {
    // If another matcher is available, we spawn a parallel progression.
    // If not, we have matched the whole concatenation.
    case match_state::MATCH:
      if (matcher_index == initial_state.size()) {
        match = true;
      } else {
        undecided = true;
        insert_next(it);
      }
      ++it;
      break;
    case match_state::FINAL_MATCH:
      if (matcher_index == initial_state.size()) {
        final_match = true;
      } else {
        undecided = true;
        insert_next(it);
      }
      // This matcher will no longer match anything, so we detach.
      it = current.erase(it);
      break;
    case match_state::UNDECIDED:
      undecided = true;
      ++it;
      break;
    // A MISMATCH state matcher is detached.
    case match_state::MISMATCH:
      it = current.erase(it);
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
void concatenate_impl<Matcher>::insert_next(
    typename std::list<current_progress>::const_iterator it) {
  using std::make_pair;
  auto matcher_index = it->second;
  current.insert(it, 
      make_pair(initial_state[matcher_index], matcher_index + 1));
}

// Consider the concatenation of "A?" with "B?".
// The following strings all match: "", "A", "B", "AB"
// Thus the current state must start out with both "A?" and "B?" in its
// list. Also, the initial state variable must be set to MATCH.
template <typename Matcher>
match_state concatenate_impl<Matcher>::initialize() {
  current.clear();

  if (initial_state.empty()) {
    return match_state::MATCH;
  }

  auto r_state = match_state::UNDECIDED;

  for (index_type index = 0; index < initial_state.size(); ++index) {
    r_state = initial_state[index].state();

    // If one of the operands starts out in the MISMATCH state, then the
    // whole concatenation will also be a MISMATCH with anything.
    if (r_state == match_state::MISMATCH) {
      current.clear();
      return match_state::MISMATCH;
    } else if (r_state != match_state::FINAL_MATCH) {
      current.push_back(std::make_pair(initial_state[index], index + 1));
    }
    if (r_state == match_state::UNDECIDED) break;
  }
  
  if (r_state == match_state::FINAL_MATCH) {
    current.clear();
  }
  return r_state;
}
}//namespace detail

// This function creates the concatenation object.
template <typename MatcherContainer>
typename MatcherContainer::value_type
concatenate(MatcherContainer&& container) {
  using matcher_type = typename MatcherContainer::value_type;
  using std::forward;

  matcher_factory<detail::concatenate_impl<matcher_type>> fac;
  return fac.create(forward<MatcherContainer>(container));
}


}//namespace lex
#endif// _concatenation_h_
