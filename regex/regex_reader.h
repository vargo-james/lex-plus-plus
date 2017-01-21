/*
 * This class reads a regex from a range and a 
 * std::regex_constants::syntax_option_type variable.
 */

#ifndef _regex_reader_h_
#define _regex_reader_h_

#include "matcher/atomic.h"
#include "matcher/matcher.h"
#include "range.h"
#include "token_stream.h"

#include <iterator>
#include <memory>
#include <regex>

namespace lex {


template <typename InputIt, typename Traits = std::regex_traits<
            typename std::iterator_traits<InputIt>::value_type>>
class regex_reader {
 public:
  using flag_type = std::regex_constants::syntax_option_type;
  using iterator = InputIt;
  using value_type = typename std::iterator_traits<InputIt>::value_type;
  using matcher_type = matcher<value_type>;

  regex_reader(InputIt b, InputIt e, 
      flag_type f = std::regex_constants::extended)
    : ts(b,e,f),
      f_ {f} {}

  matcher_type read() {return {};}
 private:
  token_stream<InputIt, Traits> ts;
  flag_type f_;

  /*
  matcher_type alternate_branches(range_type& r);
  matcher_type read_branch(range_type& r);
  */
};

/*
template <typename CharT, typename Traits>
typename regex_reader<CharT,Traits>::matcher_type
regex_reader<CharT,Traits>::alternate_branches(range_type& r) {
  using std::move;
  if (r.empty()) {
    return {};
  }
  std::vector<matcher_type> branches;
  value_type ch {'\0'};
  do {
    if (ch) r.checked_advance();
    r.set_mark();
    auto branch = read_branch(r);
    if (!r.has_advanced()) {
      break;
    }
    branches.push_back(move(branch));
    if (r.empty()) break;
    ch = read_symbol(r);
  } while (is_disjunction(ch));

  if (branches.size() == 1) {
    return branches.front();
  }
  return alternation(move(branches));
}

template <typename CharT, typename Traits>
typename regex_reader<CharT,Traits>::matcher_type
regex_reader<CharT,Traits>::read_branch(range_type& r) {
  auto b = r.get_begin();
  auto e = r.get_end();
  return ERE_branch(b,e);
}
*/

}//namespace lex
#endif// _regex_reader_
