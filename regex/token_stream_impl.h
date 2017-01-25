#ifndef _token_stream_impl_h_
#define _token_stream_impl_h_

#include "regex_range.h"
#include "context.h"
#include "regex_token.h"
#include "token_table.h"

#include <functional>
#include <iterator>
#include <locale>
#include <regex>

namespace lex {

template <typename InputIt, typename Traits>
class token_stream_impl {
 public:
  using flag_type = std::regex_constants::syntax_option_type;
  using value_type = typename regex_range<InputIt,Traits>::value_type;
  using token = regex_token<value_type,Traits>;
  using locale_type = typename Traits::locale_type;

  token_stream_impl(InputIt b, InputIt e, flag_type f)
    : source(b,e),
      loc {Traits{}.getloc()},
      flag {f} {}

  bool empty() const {return source.empty();}
  void get(token& out);
 private:
  regex_range<InputIt,Traits> source;
  locale_type loc;
  context con;
  flag_type flag;

  void get(token& out, context::site site);
  void default_get(token& out);
  void default_get_escaped(token& out);

  bool flag_includes(flag_type f) const {return flag & f;}

  void update_context(token tok) {con.update(tok.type);}

  token literal_token(value_type ch) const;

  int context_depth() const {return con.depth();}
  context::site get_context() const {return con.get();}
  bool context_first_bracket_char() const {
    return con.first_bracket_char();
  }
  bool context_after_bracket_negation() const {
    return con.after_bracket_negation();
  }
};

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::get(token& out) {
  get(out, get_context());
  update_context(out);
}

template <typename InputIt, typename Traits>
void 
token_stream_impl<InputIt,Traits>::get(token& out, context::site site) {
  switch (site) {
  case context::DEFAULT:
    default_get(out);
    break;
    /*
  case context::REPLICATION:
    replication_get(out);
    break;
  case context::SUBEXPR:
    subexpr_get(out);
    break;
  case context::BRACKET:
    bracket_get(out);
    break;
  case context::COLLATE:
    collation_get(out, value_type('.'), token_type::R_COLLATE);
    break;
  case context::EQUIV:
    collation_get(out, value_type('='), token_type::R_EQUIV);
    break;
  case context::CLASS:
    collation_get(out, value_type(':'), token_type::R_CLASS);
    break;
  */
  }
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::default_get(token& out) {
  value_type ch;
  source.get(ch);
  if (ch == value_type('\\')) {
    default_get_escaped(out);
  } else {
    const auto special_table = special_character_table<value_type>(flag);
    auto val = special_table.value(ch);
    if (val == token_type::R_PAREN && context_depth() == 0) {
      val = token_type::LITERAL;
    }
    out = (val == token_type::LITERAL)? literal_token(ch): token(ch,val);
  }
}

template <typename InputIt, typename Traits>
void token_stream_impl<InputIt,Traits>::default_get_escaped(token& out) {
  const auto escape_table = escape_character_table<value_type>(flag);
  value_type escaped_char(0);
  if (!source.get(escaped_char)) {
    out = token(value_type('\\'));
  } else {
    auto val = escape_table.value(escaped_char);
    if (val == token_type::LITERAL) {
      out = literal_token(escaped_char);
    }
    out = token(escaped_char, val);
  }
}

template <typename InputIt, typename Traits>
typename token_stream_impl<InputIt,Traits>::token
token_stream_impl<InputIt,Traits>::literal_token(value_type ch) const {
  if (flag_includes(flag_type::icase)) {
    ch = std::toupper(ch, loc);
  }
  return token(ch, token_type::LITERAL);
}

}// namespace lex
#endif// _token_stream_impl_h_
