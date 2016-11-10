#ifndef _regex_h_
#define _regex_h_

#include "regex_types.h"
#include "regex_factory_impl.h"

#include <regex>
#include <string>

namespace lex {

template <typename Char, typename Traits = std::regex_traits<Char>>
class regex {
 public:
  using char_type = Char;
  using traits_type = Traits;
  using string_type = typename Traits::string_type;
  using locale_type = typename Traits::locale_type;

  template <typename Iterator, 
            typename = enable_iterator_t<Iterator, char_type>>
  explicit regex(Iterator begin, Iterator end)
    : matcher_ {detail::extended_reg_exp<decltype(begin),Traits>(
        begin, end)} {
    if (begin != end) {
      throw regex_error("Invalid regex: could not read");
    }
  }

  template <typename ST, typename SA>
  explicit regex(const std::basic_string<Char,ST,SA>& str)
    : regex(str.begin(), str.end()) {}

  template <typename Iterator, 
            typename = enable_iterator_t<Iterator, char_type>>
  Iterator max_match(Iterator begin, Iterator end);
 private:
  matcher<Char> matcher_;
};

template <typename Char, typename Traits>
template <typename Iterator, typename>
Iterator 
regex<Char,Traits>::max_match(Iterator begin, Iterator end) {
  auto accepted = begin;
  bool loop_done {false};
  for (auto seek = begin; !loop_done && seek != end;) {
    matcher_.update(*seek);
    ++seek;
    switch (matcher_.state()) {
    case match_state::MATCH:
      accepted = seek;
      break;
    case match_state::FINAL_MATCH:
      accepted = seek;
      loop_done = true;
      break;
    case match_state::MISMATCH:
      loop_done = true;
      break;
    case match_state::UNDECIDED:
      break;
    }
  }
  // The matcher needs to be reinitialized to be used again later.
  matcher_.initialize();
  return accepted;
}
         
}//namespace lex
#endif// _regex_h_
