#ifndef _regex_replication_h_
#define _regex_replication_h_

#include "simple_regex.h"

#include <cstddef>
#include <list>
#include <utility>

namespace token_iterator {
namespace detail {

template <typename Regex>
class regex_replicator_transition
  : public regex_transition_cloner<
      regex_replicator_transition<Regex>,
      typename Regex::char_type
    > {
 public:
  using regex_type = Regex;
  using char_type = typename Regex::char_type;
  using index_type = size_t;
  using current_progress = std::pair<regex_type, index_type>;

  regex_replicator_transition(regex_type&& reg, replication rep)
    : lower {rep.lower},
      upper {rep.upper},
      regex {std::move(reg)} {}
  regex_replicator_transition(const regex_type& reg, replication rep)
    : lower {rep.lower},
      upper {rep.upper},
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
  case regex_state::FINAL_MATCH:
    return regex_state::FINAL_MATCH;
  case regex_state::UNDECIDED:
    current.push_back(std::make_pair(regex, 1));
    break;
  case regex_state::MATCH:
    current.push_back(std::make_pair(regex, 1));
    lower = 0;
    break;
  case regex_state::MISMATCH:
    return regex_state::MISMATCH;
  }
  return lower? r_state : regex_state::MATCH;
}

template <typename Regex>
regex_state 
regex_replicator_transition<Regex>::update(const char_type& ch) {
  bool match {false};
  bool final_match {false};
  bool undecided {false};

  for (auto it = current.begin(); it != current.end();) {
    it->first.update(ch);
    auto state = it->first.state();
    auto count = it->second;

    switch (state) {
    // If another regex is available, we spawn a parallel progression.
    // If not, we have matched the whole concatenation.
    case regex_state::MATCH:
      if (lower <= count && count <= upper) {
        match = true;
      } else {
        undecided = true;
      }
      if (count != upper) {
        current.insert(it, std::make_pair(regex, count + 1));
      }
      ++it;
      break;
    case regex_state::FINAL_MATCH:
      if (lower <= count && count < upper) {
        match = true;
      } else if (lower <= count && count == upper) {
        final_match = true;
      } else if (count < lower) {
        undecided = true;
      }
      if (count != upper) {
        (it->first).initialize();
        ++(it->second);
      }
      ++it;
      break;
    // A MISMATCH state is detached.
    case regex_state::MISMATCH:
      it = current.erase(it);
      break;
    default:
      undecided = true;
      ++it;
      break;
    }
  }
  if (match) return regex_state::MATCH;
  if (final_match && undecided) return regex_state::MATCH;
  if (final_match) return regex_state::FINAL_MATCH;
  if (undecided) return regex_state::UNDECIDED;
  return regex_state::MISMATCH;
}

template <typename Regex>
Regex replicate(Regex&& regex, replication rep) {
  regex_factory<regex_replicator_transition<Regex>> fac;
  return fac.create(std::forward<Regex>(regex), rep);
}
}//namespace detail
}//namespace token_iterator
#endif// _regex_replication_h_
