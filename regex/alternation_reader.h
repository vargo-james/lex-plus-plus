#ifndef _alternation_reader_h_
#define _alternation_reader_h_

#include "matcher/matcher.h"
//#include "reader_impl.h"

#include <iterator>
#include <vector>

namespace lex {


template <typename InputIt, typename Traits>
class alternation_reader : public reader_impl {
 public:
  using reader_impl::flag_type;
  using reader_impl::error_type;
  using reader_impl::value_type;
  using reader_impl::matcher_type;
  using reader_impl::stream_type;
  using reader_impl::traits_type;

  using reader_impl::reader_impl;

  bool read(matcher_type& matcher) override;
};

template <typename InputIt, typename Traits>
bool alternation_reader<InputIt,Traits>::read(matcher_type& matcher) {
  using std::move;

  if (ts.empty()) return false;

  token tok;
  std::vector<matcher_type> branches;

  for (matcher_type br; true; ) {
    if (!get_branch(br)) break;
    branches.push_back(move(br));
    if (!ts.get(tok)) break;
    if (tok.type != token_type::ALTERNATION) {
      ts.putback(tok);
      break;
    }
  }

  matcher = compose(branches, [](auto&& v) {
        return lex::alternation(move(v));
      });
  return !error();
}

}//namespace lex
#endif// _alternation_reader_h_
