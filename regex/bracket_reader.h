#ifndef _bracket_reader_h_
#define _bracket_reader_h_

#include "reader.h"
#include "token_stream.h"

namespace lex {

template <typename InputIt, typename Traits>
class bracket_reader {
 public:
  using flag_type = regex_constants::syntax_option_type;
  using error_type = regex_constants::error_type;
  using value_type = typename std::iterator_traits<InputIt>::value_type;
  using matcher_type = matcher<value_type>;
  using stream_type = token_stream<InputIt,Traits>;
  using token = regex_token<value_type,Traits>;
  using traits_type = Traits;
  using string_type = typename Traits::string_type;

  bracket_reader(traits_type& t, stream_type& s, flag_type f)
    : traits {t},
      ts {s},
      flag {f} {}

  bool read(matcher_type& matcher);

 private:
  traits_type& traits;
  stream_type& ts;
  flag_type flag;
};

template <typename InputIt, typename Traits>
bool bracket_reader<InputIt,Traits>::read(matcher_type& matcher) {
  return false;
}

}//namespace lex
#endif// _bracket_reader_h_
