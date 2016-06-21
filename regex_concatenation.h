#ifndef _regex_concatenation_h_
#define _regex_concatenation_h_

#include <cstddef>
#include <list>
#include <vector>
#include <utility>

#include "simple_regex.h"

#include <iostream>
namespace token_iterator {
// We need to handle concatenation of a list of regexes.
// We need to keep a list of all the individual regexes in their
// initial states.
// The current state will be maintained by a list of regexes.
// Each entry in the list represents a parallel progression through
// the concatenation.
template <typename Regex>
class concatenate_transition 
  : public regex_state_transition_CRTP<
             concatenate_transition<Regex>, typename Regex::char_type
           > {
 public:
  using regex_type = Regex;
  using char_type = typename regex_type::char_type;
  using index_type = size_t;
  using current_progress = std::pair<regex_type, index_type>;

  template <typename Iterator>
  concatenate_transition(Iterator begin, Iterator end)
    : initial_state(begin, end) {}
  concatenate_transition(const std::vector<regex_type>& container)
    : initial_state(container) {}
  concatenate_transition(std::vector<regex_type>&& container)
    : initial_state(std::move(container)) {}

  regex_state update(const char_type& ch) override;
  regex_state initialize() override;
 private:
  std::vector<regex_type> initial_state;
  std::list<current_progress> current;
};

// To understand this implementation consider the concatenation of
// Left = Regex("[AB]+") with 
// Right = Regex("[BC]{2}"). 
// i.e. Concatenation(Left, Right) = Regex("[AB]+[BC]{2}");
// Regexes are spawned in their initial state.
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
// Step 5 'C': Right (MATCH)       -> nothing left to spawn. 
//                                    state = MATCH.
// Step 6 'B': Right (MISMATCH)    -> detached. list is now empty.
//                                    state = MISMATCH
template <typename Regex>
regex_state 
concatenate_transition<Regex>::update(const char_type& ch) {
  auto return_state = regex_state::UNDECIDED;

  for (auto it = current.begin(); it != current.end();) {
    it->first.update(ch);
    auto state = it->first.state();
    // regex_index points to the next regex in the initial_state vector.
    auto regex_index = it->second;
    switch (state) {
    // If another regex is available, we spawn a parallel progression.
    // If not, we have matched the whole concatenation.
    case regex_state::MATCH:
      if (regex_index == initial_state.size()) {
        std::cout << "END\n";
        return_state = regex_state::MATCH;
      } else {
        std::cout << "PUSHING BACK " << regex_index << "\n";
        current.insert(it, std::make_pair(initial_state[regex_index], 
              regex_index + 1));
      }
      ++it;
      break;
    // A MISMATCH state is detached.
    case regex_state::MISMATCH:
      it = current.erase(it);
      break;
    default:
      ++it;
      break;
    }
  }
  return current.empty()? regex_state::MISMATCH : return_state;
}

// Consider the concatenation of "A?" with "B?".
// The following strings all match: "", "A", "B", "AB"
// Thus the current state must start out with both "A?" and "B?" in its
// list. Also, the initial state variable must be set to MATCH.
template <typename Regex>
regex_state concatenate_transition<Regex>::initialize() {
  current.clear();

  if (initial_state.empty()) {
    return regex_state::MATCH;
  }

  regex_state state {regex_state::UNDECIDED};
  for (index_type index = 0; index < initial_state.size(); ++index) {
    current.push_back(std::make_pair(initial_state[index], index + 1));
    state = initial_state[index].state();
    if (state != regex_state::MATCH) break;
  }
  if (state == regex_state::MISMATCH) current.clear();
  return state;
}

// This function creates the concatenation object.
template <typename Regex>
Regex concatenate(std::vector<Regex>&& container) {
  using regex_type = Regex;
  using std::make_unique;
  using std::move;

  std::cout << "Concatenating : ";
  std::cout << container.size() << " elements\n";
  if (container.empty()) return {};
  if (container.size() == 1) return move(container[0]);
  return regex_type(
      make_unique<concatenate_transition<regex_type>>(move(container)));
}
}//namespace token_iterator
#endif// _regex_concatenation_h_
