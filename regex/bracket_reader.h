#ifndef _bracket_reader_h_
#define _bracket_reader_h_

#include "matcher/matcher.h"
#include "regex_types.h"
#include "token_source.h"

namespace lex {

template <typename Source>
class bracket_reader : public regex_constants {
 public:
  using char_type = typename Source::value_type;
  using matcher_type = matcher<char_type>;

  bracket_reader(token_source<Source>& src, syntax_option_type syntax)
    : source {src},
      flag {syntax} {}

  optional<matcher_type> read();

 private:
  token_source<Source>& source;
  syntax_option_type flag;
};

template <typename Source>
optional<typename bracket_reader<Source>::matcher_type> 
bracket_reader<Source>::read() {
  return {};
}

}//namespace lex
#endif// _bracket_reader_h_
