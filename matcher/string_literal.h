#ifndef _string_literal_h_
#define _string_literal_h_

#include "matcher/matcher.h"

namespace lex {

template <typename Iterator>
class string_matcher_transition 
  : public matcher_transition_cloner<string_matcher_transition<Iterator>, 
      value_type_t<Iterator>> {
 public:
  using value_type = value_type_t<Iterator>;
  using string_type = std::vector<value_type>;
  using size_type = typename string_type::size_type;

  string_matcher_transition(Iterator begin, Iterator end)
    : string(begin, end),
      current {0} {}

  string_matcher_transition(string_type&& str) 
    : string {std::move(str)}, 
      current {0} {}

  match_state update(const value_type& t) override;
  match_state initialize() override;
 private:
  string_type string;
  size_type current;
};  

template <typename Iterator> 
match_state 
string_matcher_transition<Iterator>::update(const value_type& t) {
  if (t == string[current]) {
    ++current;
    if (current == string.size()) return match_state::FINAL_MATCH;
    return match_state::UNDECIDED;
  }
  return match_state::MISMATCH;
}

template <typename Iterator>
match_state string_matcher_transition<Iterator>::initialize() {
  current = 0;
  return string.empty()? match_state::FINAL_MATCH : match_state::UNDECIDED;
}

template <typename Iterator>
matcher<value_type_t<Iterator>>
string_matcher(Iterator begin, Iterator end) {
  matcher_factory<string_matcher_transition<Iterator>> fac;
  return fac.create(begin, end);
}

template <typename String>
matcher<typename String::value_type>
string_matcher(String&& str) {
  return string_matcher(str.begin(), str.end());
}

}//namespace
#endif// _string_literal_h_
