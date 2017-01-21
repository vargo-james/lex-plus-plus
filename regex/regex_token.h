#ifndef _regex_token_h_
#define _regex_token_h_

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
  L_ANCHOR, R_ANCHOR, // ^$
  WORD_BOUNDARY, NEG_WORD_BOUNDARY, // \b \B
  ALTERNATION // |
};

template <typename CharT>
struct regex_token {
  using value_type = CharT;

  regex_token() = default;
  regex_token(value_type val) : value{val}, type{token_type::LITERAL} {}
  regex_token(value_type val, token_type t) : value{val}, type{t} {}

  value_type value;
  token_type type;
};

}//namespace lex
#endif// _regex_token_h_
