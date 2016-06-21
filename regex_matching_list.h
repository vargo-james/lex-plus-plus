#ifndef _regex_matching_list_h_
#define _regex_matching_list_h_

#include "regex_definition.h"

#include <algorithm>
#include <list>
#include <iterator>
#include <memory>
#include <vector>

#include <iostream>
namespace token_iterator {

template <typename RegexIterator>
regex_state matching_list_state_impl(RegexIterator begin, 
    RegexIterator end, bool matching) {
  std::cout << "AnyOf\n";
  auto initial_state = matching? regex_state::MISMATCH : regex_state::MATCH;
  auto state = initial_state;

  for (auto it = begin; it != end; ++it) {
    if (it->state() == regex_state::MATCH) {
      return flip_state(initial_state);
    }
    if (it->state() == regex_state::UNDECIDED) {
      state = regex_state::UNDECIDED;
    }
  }
  return state;
}

// This defines the transition function for the bracket combination.
template <typename Regex, bool Matching>
class matching_list_transition 
  : public regex_state_transition_CRTP<
             matching_list_transition<Regex,Matching>,
             typename Regex::char_type
           > {
 public:
  using regex_type = Regex;
  using char_type = typename Regex::char_type;
  //using typename regex_state_transition<char_type>::pointer;

  template <typename Iterator>
  matching_list_transition(Iterator begin, Iterator end)
    : initial_state(begin, end) {}
  matching_list_transition(const std::vector<regex_type>& container)
    : initial_state {container} {}
  matching_list_transition(std::vector<regex_type>&& container)
    : initial_state {std::move(container)} {}

  regex_state update(const char_type&) override;
  regex_state initialize() override;
 private:
  std::vector<regex_type> initial_state;
  std::list<regex_type> regexes;
};

template <typename Regex, bool Matching>
regex_state 
matching_list_transition<Regex, Matching>::update(
    const char_type& ch) {
  std::cout << "AnyOfTransition\n";
  auto current_state = regex_state::UNDECIDED;
  // matching_state is the state that results when at least one member
  // of the list matches.
  auto matching_state = Matching? regex_state::MATCH : 
    regex_state::MISMATCH;
  for (auto it = regexes.begin(); it != regexes.end();) {
    std::cout << "LIST UPDATE: ";
    it->update(ch);
    auto state = it->state();
    switch (state) {
    // If one match is found, the regex is a match. But we still need
    // to update the rest of the regexes.
    case regex_state::MATCH:
      current_state = matching_state;
      ++it;
      break;
    // If a mismatch is found, that regex will not need to be 
    // considered henceforth, so we may remove it.
    case regex_state::MISMATCH:
      it = regexes.erase(it);
      break;
    default:
      ++it;
      break;
    }
  }
  // We only have a mismatch when every regex in the list was a mismatch.
  // Thus they would have all been detached from the list leaving it empty.
  return regexes.empty()? flip_state(matching_state) : current_state;
}

template <typename Regex, bool Matching>
regex_state
matching_list_transition<Regex,Matching>::initialize() {
  using std::for_each;

  regexes.clear();

  std::for_each(initial_state.begin(), initial_state.end(),
      [this] (regex_type& r) {
        if (r.state() != regex_state::MISMATCH) {
          this->regexes.push_back(r);
        }
      });

  return matching_list_state_impl(initial_state.begin(), 
      initial_state.end(), Matching);
}

template <typename Regex, bool Matching>
Regex matching_list_impl(const std::vector<Regex>& container) {
  return Regex(
      std::make_unique<matching_list_transition<Regex,Matching>>(
        container));
}

template <typename Regex, bool Matching>
Regex matching_list_impl(std::vector<Regex>&& container) {
  return Regex(
      std::make_unique<matching_list_transition<Regex,Matching>>(
        std::move(container)));
}

template <typename Regex>
Regex matching_list(const std::vector<Regex>& container) {
  return matching_list_impl<Regex,true>(container);
}
template <typename Regex>
Regex matching_list(std::vector<Regex>&& container) {
  return matching_list_impl<Regex,true>(container);
}
template <typename Regex>
Regex non_matching_list(const std::vector<Regex>& container) {
  return matching_list_impl<Regex,false>(container);
}
template <typename Regex>
Regex non_matching_list(std::vector<Regex>&& container) {
  return matching_list_impl<Regex,false>(container);
}
}//token_iterator
#endif// _regex_operations_h_
