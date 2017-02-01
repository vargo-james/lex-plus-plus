#ifndef _reader_impl_h_
#define _reader_impl_h_

#include "token_stream.h"
#include "regex_types.h"

#include <iterator>

namespace lex {

template <typename InputIt, typename Traits>
class reader_impl {
 public:
  using flag_type = regex_constants::syntax_option_type;
  using error_type = regex_constants::error_type;
  using value_type = typename std::iterator_traits<InputIt>::value_type;
  using matcher_type = matcher<value_type>;
  using stream_type = token_stream<InputIt, Traits>;
  using traits_type = Traits;

  reader_impl(stream_type& s, error_type& ec, traits_type& tr, flag_type f)
    : ts {s},
      ec_ {ec},
      traits_ {tr},
      flag {f} {}

  virtual bool read(matcher_type& matcher) = 0;
  error_type error() const;
  void set_error(error_type ec);
 private:
  stream_type& ts;
  error_type& ec_;
  traits_type& traits_;
  flag_type flag;
};

}//namespace lex
#endif// _reader_impl_h_
