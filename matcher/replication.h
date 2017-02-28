#ifndef _replication_h_
#define _replication_h_

#include "matcher/matcher.h"

#include <cstddef>
#include <list>
#include <utility>

namespace lex {
namespace detail {

template <typename Matcher>
class matcher_replicator_impl
  : public matcher_impl_cloner<
      matcher_replicator_impl<Matcher>,
      typename Matcher::value_type, typename Matcher::traits_type
    > {
 public:
  using matcher_type = Matcher;
  using value_type = typename Matcher::value_type;
  using index_type = std::size_t;
  using current_progress = std::pair<matcher_type, index_type>;

  matcher_replicator_impl(matcher_type&& reg, replication_data rep)
    : lower {rep.lower},
      upper {rep.upper},
      matcher {std::move(reg)} {}
  matcher_replicator_impl(const matcher_type& reg, replication_data rep)
    : lower {rep.lower},
      upper {rep.upper},
      matcher {reg} {}
    
  match_state update(value_type ch) override;
  match_state initialize() override;
 private:
  std::size_t lower;
  std::size_t upper;
  matcher_type matcher;
  std::list<current_progress> current;
};

template <typename Matcher>
match_state matcher_replicator_impl<Matcher>::initialize() {
  current.clear();

  auto r_state = matcher.state();

  switch (r_state) {
  case match_state::FINAL_MATCH:
    return match_state::FINAL_MATCH;
  case match_state::UNDECIDED:
    current.push_back(std::make_pair(matcher, 1));
    break;
  case match_state::MATCH:
    current.push_back(std::make_pair(matcher, 1));
    lower = 0;
    break;
  case match_state::MISMATCH:
    return match_state::MISMATCH;
  }
  return lower? r_state : match_state::MATCH;
}

template <typename Matcher>
match_state 
matcher_replicator_impl<Matcher>::update(value_type ch) {
  bool match {false};
  bool final_match {false};
  bool undecided {false};

  for (auto it = current.begin(); it != current.end();) {
    it->first.update(ch);
    auto state = it->first.state();
    auto count = it->second;

    switch (state) {
    // If another matcher is available, we spawn a parallel progression.
    // If not, we have matched the whole concatenation.
    case match_state::MATCH:
      if (lower <= count && count <= upper) {
        match = true;
      } else {
        undecided = true;
      }
      if (count != upper) {
        current.insert(it, std::make_pair(matcher, count + 1));
      }
      ++it;
      break;
    case match_state::FINAL_MATCH:
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
    case match_state::MISMATCH:
      it = current.erase(it);
      break;
    default:
      undecided = true;
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
}//namespace detail

template <typename Matcher>
Matcher replicate(Matcher&& matcher, replication_data rep) {
  //if (rep.lower == rep.upper == 1) return matcher;
  matcher_factory<detail::matcher_replicator_impl<Matcher>> fac;
  return fac.create(std::forward<Matcher>(matcher), rep);
}
}//namespace lex
#endif// _replication_h_
