#ifndef _string_literal_h_
#define _string_literal_h_

#include "matcher/matcher.h"

#include <utility>

namespace lex {

template <typename Iterator, typename Traits>
class string_matcher_impl 
  : public matcher_impl_cloner<string_matcher_impl<Iterator, Traits>, 
      value_type_t<Iterator>, Traits> {
 public:
  using value_type = value_type_t<Iterator>;
  using traits_type = Traits;
  using string_type = typename Traits::string_type;
  using size_type = typename string_type::size_type;

  string_matcher_impl(Iterator begin, Iterator end, const Traits& tr)
    : string(begin, end),
      current {0},
      traits_ {tr} {}

  string_matcher_impl(string_type str, const Traits& tr) 
    : string {std::move(str)}, 
      current {0},
      traits_ {tr} {}

  match_state update(value_type t) override;
  match_state initialize() override;
 private:
  string_type string;
  size_type current;
  const traits_type& traits_;
};  

template <typename Iterator, typename Traits> 
match_state 
string_matcher_impl<Iterator, Traits>::update(value_type t) {
  if (t == string[current]) {
    ++current;
    if (current == string.size()) {
      return match_state::FINAL_MATCH;
    }
    return match_state::UNDECIDED;
  }
  return match_state::MISMATCH;
}

template <typename Iterator, typename Traits>
match_state string_matcher_impl<Iterator, Traits>::initialize() {
  current = 0;
  return string.empty()? match_state::FINAL_MATCH : match_state::UNDECIDED;
}

template <typename String, typename Traits>
matcher<typename String::value_type, Traits>
string_matcher(String&& str, const Traits& traits) {
  using Iterator = typename String::const_iterator;
  matcher_factory<string_matcher_impl<Iterator, Traits>> fac;
  return fac.create(std::forward<String>(str), traits);
}

}//namespace
#endif// _string_literal_h_
