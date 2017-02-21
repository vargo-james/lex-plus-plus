#ifndef _regex_token_h_
#define _regex_token_h_

#include <cstddef>

namespace lex {

enum class token_type {
  ALTERNATION,

  // Expression tokens
  LITERAL, CONTROL_CHAR, HEX_ESCAPE, UNICODE_ESCAPE, // Forms of literals
  STRING_LITERAL,
  CONTROL,
  CHAR_CLASS,
  ASSERTION,
  SIMPLE_BACK_REF, // \4
  COMPOUND_BACK_REF, // \41
  BACK_REF,     // Either of the above two.
  L_PAREN, R_PAREN, 
  NO_SUBEXP, // (?=) ?! ?:
  L_BRACKET, // This is read by an expression_reader, not a bracket_reader

  // Error tokens
  TRAILING_ESCAPE,
  REDUNDANT_ESCAPE,
  
  // Bracket tokens
  R_BRACKET, 
  NEGATION, 
  RANGE_DASH, // [^2-3] 
  BRACKET_LITERAL,
  COLLATE,
  EQUIV,
  CLASS,
  L_COLLATE, R_COLLATE, L_CLASS, R_CLASS, L_EQUIV, R_EQUIV,//Will be deprecated

  // Replication tokens
  REPLICATION,  // ?+*
  L_BRACE, 
  R_BRACE, 
  COMMA, 
  COUNT,
  REP_LOWER,
  REP_UPPER,

  // Error token
  BADBRACE,
};

inline bool is_literal(token_type type) {
  return type == token_type::LITERAL ||
    type == token_type::HEX_ESCAPE ||
    type == token_type::UNICODE_ESCAPE ||
    type == token_type::CONTROL_CHAR;
}

template <typename CharT, typename Traits>
struct regex_token {
  using string_type = typename Traits::string_type;
  using value_type = CharT;
  using label_type = token_type;

  regex_token() = default;
  regex_token(value_type c)
    : ch {c} {}
  regex_token(value_type c, token_type t)
    : ch {c}, 
      type {t} {}
  regex_token(const string_type& s, token_type t)
    : str {s},
      type {t} {}
  regex_token(std::size_t cnt, token_type t)
    : count {cnt},
      type {t} {}

  value_type ch {'\0'};
  std::size_t count {0};
  string_type str;
  label_type type {token_type::LITERAL};
};

template <typename CharT, typename Traits>
inline bool operator==(const regex_token<CharT,Traits>& left,
    const regex_token<CharT,Traits>& right) {
  return left.ch == right.ch && left.type == right.type && 
    left.count == right.count && left.str == right.str;
}

template <typename CharT, typename Traits>
inline bool operator!=(const regex_token<CharT,Traits>& left,
    const regex_token<CharT,Traits>& right) {
  return !(left == right);
}

}//namespace lex
#endif// _regex_token_h_
