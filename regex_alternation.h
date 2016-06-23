#ifndef _regex_alternation_h_
#define _regex_alternation_h_

#include "simple_regex.h"

#include <algorithm>
#include <list>
#include <vector>
#include <utility>

namespace token_iterator {
namespace detail {

template <typename RegexIterator>
regex_state alternation_state(RegexIterator begin, 
    RegexIterator end) {
  auto state = regex_state::MISMATCH;

  bool undecided;
  for (auto it = begin; it != end; ++it) {
    if (it->state() == regex_state::MATCH) {
      return regex_state::MATCH;
    }
    if (it->state() == regex_state::FINAL_MATCH) {
      state = regex_state::FINAL_MATCH;
    }
    if (it->state() == regex_state::UNDECIDED) {
      undecided = true;
    }
  }
  
  if (undecided && state == regex_state::FINAL_MATCH) {
    return regex_state::MATCH;
  }
  return undecided? regex_state::UNDECIDED : state;
}

// This defines the transition function for the bracket combination.
template <typename Regex>
class alternation_transition 
  : public regex_transition_cloner<
             alternation_transition<Regex>, typename Regex::char_type
           > {
 public:
  using regex_type = Regex;
  using char_type = typename Regex::char_type;

  alternation_transition(const std::vector<regex_type>& c)
    : initial_state {c} {}
  alternation_transition(std::vector<regex_type>&& c)
    : initial_state {std::move(c)} {}

  regex_state update(const char_type&) override;
  regex_state initialize() override;
 private:
  std::vector<regex_type> initial_state;
  std::list<regex_type> regexes;
};

template <typename Regex>
regex_state alternation_transition<Regex>::update(const char_type& ch) {
  // These flags track what we have seen after traversing the list.
  bool undecided {false};
  bool final_match {false};
  bool match {false};

  for (auto it = regexes.begin(); it != regexes.end();) {
    it->update(ch);
    auto state = it->state();
    switch (state) {
    // If one match is found, the regex is a match. But we still need
    // to update the rest of the regexes.
    case regex_state::MATCH:
      match = true;
      ++it;
      break;
    // If a FINAL_MATCH is found this regex will not need to be looked
    // at again.
    case regex_state::FINAL_MATCH:
      final_match = true;
      it = regexes.erase(it);
      break;
    case regex_state::UNDECIDED:
      undecided = true;
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
  if (match) {
    return regex_state::MATCH;
  } else if (final_match && undecided) {
    return regex_state::MATCH;
  } else if (final_match) {
    return regex_state::FINAL_MATCH;
  } else {
    return undecided? regex_state::UNDECIDED : regex_state::MISMATCH;
  }
}

template <typename Regex>
regex_state alternation_transition<Regex>::initialize() {
  using std::for_each;

  regexes.clear();

  for_each(initial_state.begin(), initial_state.end(),
      [this] (regex_type& r) {
        if (r.state() != regex_state::MISMATCH) {
          this->regexes.push_back(r);
        }
      });

  return alternation_state(initial_state.begin(), initial_state.end());
}

template <typename RegexContainer>
typename RegexContainer::value_type
alternation(RegexContainer&& container) {
  using regex_type = typename RegexContainer::value_type;

  regex_factory<alternation_transition<regex_type>> fac;
  return fac.create(std::forward<RegexContainer>(container));
}

}//namespace detail
}//namespace token_iterator
#endif// _regex_alternation_h_
