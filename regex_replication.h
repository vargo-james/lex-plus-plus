#ifndef _regex_replication_h_
#define _regex_replication_h_

#include "regex_definition.h"

#include <cstddef>
#include <list>
#include <utility>

#include <iostream>
namespace token_iterator {
struct replicator {
  replicator(size_t l, size_t u): lower {l}, upper {u} {
    std::cout << lower << ' ' << upper << '\n';
  }
  size_t lower;
  size_t upper;
};


template <typename Regex>
class regex_replicator_transition
  : public regex_state_transition_CRTP<
      regex_replicator_transition<Regex>,
      typename Regex::char_type
    > {
 public:
  using regex_type = Regex;
  using char_type = typename Regex::char_type;
  using index_type = size_t;
  using current_progress = std::pair<regex_type, index_type>;

  regex_replicator_transition(regex_type&& reg, replicator replicate)
    : lower {replicate.lower},
      upper {replicate.upper},
      regex {std::move(reg)} {}
  regex_replicator_transition(const regex_type& reg, replicator replicate)
    : lower {replicate.lower},
      upper {replicate.upper},
      regex {reg} {}
    
  regex_state update(const char_type& ch) override;
  regex_state initialize() override;
 private:
  size_t lower;
  size_t upper;
  regex_type regex;
  std::list<current_progress> current;
};

template <typename Regex>
regex_state regex_replicator_transition<Regex>::initialize() {
  current.clear();

  auto r_state = regex.state();

  switch (r_state) {
  case regex_state::UNDECIDED:
    current.push_back(std::make_pair(regex, 1));
    break;
  case regex_state::MATCH:
    current.push_back(std::make_pair(regex, 1));
    lower = 0;
    break;
  case regex_state::MISMATCH:
    break;
  }
  return lower? r_state : regex_state::MATCH;
}

template <typename Regex>
regex_state 
regex_replicator_transition<Regex>::update(const char_type& ch) {
  auto return_state = regex_state::UNDECIDED;

  for (auto it = current.begin(); it != current.end();) {
    it->first.update(ch);
    auto state = it->first.state();
    auto count = it->second;

    switch (state) {
    // If another regex is available, we spawn a parallel progression.
    // If not, we have matched the whole concatenation.
    case regex_state::MATCH:
      if (lower <= count && count <= upper) {
        return_state = regex_state::MATCH;
      } 
      if (count != upper) {
        current.insert(it, std::make_pair(regex, count + 1));
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

template <typename Char>
simple_regex<Char> 
replicate(simple_regex<Char>&& regex, replicator rep) {
  using regex_type = simple_regex<Char>;
  using std::make_unique;

  return regex_type(
      make_unique<regex_replicator_transition<regex_type>>(
        std::move(regex), rep)
      );
}

}//namespace token_iterator
#endif// _regex_replication_h_
