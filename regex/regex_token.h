#ifndef _regex_token_h_
#define _regex_token_h_

#include <cstddef>

namespace lex {

enum class token_type {
  LITERAL,
  ANY_CHAR,     // .
  BACK_REF,     // \3
  REPLICATION,  // ?+*
  L_BRACE, R_BRACE, COUNT_SEP, DIGIT, // {2,3}
  L_PAREN, R_PAREN, POS_LOOKAHEAD, NEG_LOOKAHEAD, NO_SUBEXP, // (?=) ?! ?:
  L_BRACKET, R_BRACKET, NEGATION, RANGE_DASH, // [^2-3] 
  L_COLLATE, R_COLLATE, L_CLASS, R_CLASS, L_EQUIV, R_EQUIV, // [..] [::] [==]
  CHAR_CLASS,
  ASSERTION,
  CONTROL_CHAR,
  CONTROL,
  HEX_ESCAPE,
  UNICODE_ESCAPE,
  ALTERNATION,
  COUNT // |
};

template <typename CharT, typename Traits>
struct regex_token {
  using string_type = typename Traits::string_type;
  using value_type = CharT;

  regex_token() = default;
  regex_token(value_type c): ch {c}, type {token_type::LITERAL}, count {0} {}
  regex_token(value_type c, token_type t): ch {c}, type {t}, count {0} {}
  regex_token(const string_type& s, token_type t)
    : ch {'\0'}, 
      type {t}, 
      str {s}, 
      count {0} {}
  regex_token(std::size_t cnt, token_type t)
    : ch {'\0'}, 
      type {t}, 
      count {cnt} {}

  value_type ch;
  token_type type;
  string_type str;
  std::size_t count;
};

}//namespace lex
#endif// _regex_token_h_
