#ifndef _regex_matching_list_h_
#define _regex_matching_list_h_

#include "simple_regex.h"

#include <algorithm>
#include <list>
#include <iterator>
#include <memory>
#include <vector>

#include <iostream>
namespace token_iterator {
namespace detail {

template <typename RegexIterator>
regex_state matching_list_state(RegexIterator begin, 
    RegexIterator end) {
  std::cout << "AnyOf\n";
  auto state = regex_state::MISMATCH;

  for (auto it = begin; it != end; ++it) {
    if (it->state() == regex_state::MATCH) {
      return regex_state::MATCH;
    }
    if (it->state() == regex_state::UNDECIDED) {
      state = regex_state::UNDECIDED;
    }
  }
  return state;
}

// This defines the transition function for the bracket combination.
template <typename Regex>
class matching_list_transition 
  : public regex_state_transition_CRTP<
             matching_list_transition<Regex>,
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

template <typename Regex>
regex_state 
matching_list_transition<Regex>::update(
    const char_type& ch) {
  std::cout << "AnyOfTransition\n";
  auto current_state = regex_state::UNDECIDED;

  for (auto it = regexes.begin(); it != regexes.end();) {
    std::cout << "LIST UPDATE: ";
    it->update(ch);
    auto state = it->state();
    switch (state) {
    // If one match is found, the regex is a match. But we still need
    // to update the rest of the regexes.
    case regex_state::MATCH:
      current_state = regex_state::MATCH;
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
  return regexes.empty()? regex_state::MISMATCH : current_state;
}

template <typename Regex>
regex_state
matching_list_transition<Regex>::initialize() {
  using std::for_each;

  regexes.clear();

  std::for_each(initial_state.begin(), initial_state.end(),
      [this] (regex_type& r) {
        if (r.state() != regex_state::MISMATCH) {
          this->regexes.push_back(r);
        }
      });

  return matching_list_state(initial_state.begin(), 
      initial_state.end());
}

template <typename Regex>
Regex matching_list(const std::vector<Regex>& container) {
  return Regex(
      std::make_unique<matching_list_transition<Regex>>(
        container));
}

template <typename Regex>
Regex matching_list(std::vector<Regex>&& container) {
  return Regex(
      std::make_unique<matching_list_transition<Regex>>(
        std::move(container)));
}

}//namespace detail
}//namespace token_iterator
#endif// _regex_operations_h_
