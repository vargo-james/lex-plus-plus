#ifndef _character_markup_tables_h_
#define _character_markup_tables_h_

#include "markup_table.h"
#include "regex_token.h"
#include "regex_types.h"

#include <locale>

namespace lex {

template <typename CharT, typename Traits>
markup_table<CharT, token_type> basic_specials(const Traits& t) {
  return  markup_table<CharT, token_type>({
    {CharT('.'), token_type::CHAR_CLASS},
    {CharT('['), token_type::L_BRACKET},
    {CharT('*'), token_type::REPLICATION},
    {CharT('^'), token_type::ASSERTION},
    {CharT('$'), token_type::ASSERTION},
    {CharT('\\'), token_type::TRAILING_ESCAPE}
  }, token_type::LITERAL);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> basic_escapes(const Traits& t) {
  return markup_table<CharT, token_type>({
      {CharT('.'), token_type::LITERAL},
      {CharT('['), token_type::LITERAL},
      {CharT('*'), token_type::LITERAL},
      {CharT('^'), token_type::LITERAL},
      {CharT('$'), token_type::LITERAL},
      {CharT('\\'), token_type::LITERAL},
      {CharT('('), token_type::L_PAREN},
      {CharT(')'), token_type::R_PAREN},
      {CharT('{'), token_type::L_BRACE},
      {CharT('+'), token_type::REPLICATION},
      {CharT('?'), token_type::REPLICATION},
      {CharT('|'), token_type::ALTERNATION},
    }, [&t](auto val) {return std::isdigit(val, t.getloc());}, 
    token_type::SIMPLE_BACK_REF, token_type::REDUNDANT_ESCAPE);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> extended_specials(const Traits& t) {
  return  markup_table<CharT, token_type>({
    {CharT('.'), token_type::CHAR_CLASS},
    {CharT('['), token_type::L_BRACKET},
    {CharT('*'), token_type::REPLICATION},
    {CharT('^'), token_type::ASSERTION},
    {CharT('$'), token_type::ASSERTION},
    {CharT('('), token_type::L_PAREN},
    {CharT(')'), token_type::R_PAREN},
    {CharT('{'), token_type::L_BRACE},
    {CharT('+'), token_type::REPLICATION},
    {CharT('?'), token_type::REPLICATION},
    {CharT('|'), token_type::ALTERNATION},
    {CharT('\\'), token_type::TRAILING_ESCAPE}
  }, token_type::LITERAL);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> extended_escapes(const Traits& t) {
  return  markup_table<CharT, token_type>({
    {CharT('.'), token_type::LITERAL},
    {CharT('['), token_type::LITERAL},
    {CharT('*'), token_type::LITERAL},
    {CharT('^'), token_type::LITERAL},
    {CharT('$'), token_type::LITERAL},
    {CharT('('), token_type::LITERAL},
    {CharT(')'), token_type::LITERAL},
    {CharT('{'), token_type::LITERAL},
    {CharT('+'), token_type::LITERAL},
    {CharT('?'), token_type::LITERAL},
    {CharT('|'), token_type::LITERAL},
    {CharT('\\'), token_type::LITERAL},
  }, [&t](auto val) {return std::isdigit(val, t.getloc());}, 
  token_type::SIMPLE_BACK_REF, token_type::REDUNDANT_ESCAPE);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> ECMAScript_specials(const Traits& t) {
  return extended_specials<CharT,Traits>(t);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> ECMAScript_escapes(const Traits& t) {
  return  markup_table<CharT, token_type>({
    {CharT('.'), token_type::LITERAL},
    {CharT('['), token_type::LITERAL},
    {CharT('*'), token_type::LITERAL},
    {CharT('^'), token_type::LITERAL},
    {CharT('$'), token_type::LITERAL},
    {CharT('('), token_type::LITERAL},
    {CharT(')'), token_type::LITERAL},
    {CharT('{'), token_type::LITERAL},
    {CharT('+'), token_type::LITERAL},
    {CharT('?'), token_type::LITERAL},
    {CharT('|'), token_type::LITERAL},
    {CharT('\\'), token_type::LITERAL},
    {CharT('x'), token_type::HEX_ESCAPE},
    {CharT('u'), token_type::UNICODE_ESCAPE},
    {CharT('w'), token_type::CHAR_CLASS},
    {CharT('W'), token_type::CHAR_CLASS},
    {CharT('d'), token_type::CHAR_CLASS},
    {CharT('D'), token_type::CHAR_CLASS},
    {CharT('s'), token_type::CHAR_CLASS},
    {CharT('S'), token_type::CHAR_CLASS},
    {CharT('l'), token_type::CHAR_CLASS},
    {CharT('L'), token_type::CHAR_CLASS},
    {CharT('u'), token_type::CHAR_CLASS},
    {CharT('U'), token_type::CHAR_CLASS},
    {CharT('f'), token_type::CONTROL_CHAR},
    {CharT('n'), token_type::CONTROL_CHAR},
    {CharT('r'), token_type::CONTROL_CHAR},
    {CharT('t'), token_type::CONTROL_CHAR},
    {CharT('v'), token_type::CONTROL_CHAR},
    {CharT('0'), token_type::CONTROL_CHAR},
    {CharT('c'), token_type::CONTROL},
    {CharT('b'), token_type::ASSERTION},
    {CharT('B'), token_type::ASSERTION},
  }, [&t](auto val) {return std::isdigit(val, t.getloc());}, 
          token_type::COMPOUND_BACK_REF, token_type::LITERAL);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> basic_rep_specials(const Traits& t) {
  return markup_table<CharT,token_type>({
      {CharT(','), token_type::COMMA},
      }, [&t](auto val) {return std::isdigit(val, t.getloc());}, 
      token_type::COUNT, token_type::BADBRACE);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> basic_rep_escapes(const Traits& t) {
  return markup_table<CharT,token_type>({
      {CharT('}'), token_type::R_BRACE},
      }, token_type::BADBRACE);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> erep_specials(const Traits& t) {
  return markup_table<CharT,token_type>({
      {CharT(','), token_type::COMMA},
      {CharT('}'), token_type::R_BRACE}
      }, [&t](auto val) {return std::isdigit(val, t.getloc());}, 
      token_type::COUNT, token_type::BADBRACE);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> erep_escapes(const Traits& t) {
  return markup_table<CharT,token_type>({}, token_type::BADBRACE);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> subexpression_specials(
    regex_constants::syntax_option_type syntax, const Traits& t) {
  return {};
  //return markup_table<CharT,token_type>({}, token_type::LITERAL);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> subexpression_escapes(
    regex_constants::syntax_option_type syntax, const Traits& t) {
  return markup_table<CharT,token_type>({
      {CharT('='), token_type::ASSERTION},
      {CharT('!'), token_type::ASSERTION},
      {CharT('?'), token_type::NO_SUBEXP},
      }/*, token_type::LITERAL*/);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> bracket_specials(
    regex_constants::syntax_option_type syntax, const Traits& t) {
  return markup_table<CharT,token_type>({
      {CharT('^'), token_type::NEGATION},
      {CharT(']'), token_type::R_BRACKET},
      {CharT('-'), token_type::RANGE_DASH},
     }, token_type::BRACKET_LITERAL);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> bracket_escapes(
    regex_constants::syntax_option_type syntax, const Traits& t) {
  return markup_table<CharT,token_type>({
      {CharT(':'), token_type::CLASS},
      {CharT('='), token_type::EQUIV},
      {CharT('.'), token_type::COLLATE},
      }/*, token_type::BRACKET_LITERAL*/);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> brace_specials(
    regex_constants::syntax_option_type syntax, const Traits& t) {
  using flag_type = regex_constants::syntax_option_type;
  bool basic = (syntax & flag_type::basic) || (syntax & flag_type::grep);

  return basic? basic_rep_specials<CharT,Traits>(t) : 
    erep_specials<CharT,Traits>(t);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> brace_escapes(
    regex_constants::syntax_option_type syntax, const Traits& t) {
  using flag_type = regex_constants::syntax_option_type;
  bool basic = (syntax & flag_type::basic) || (syntax & flag_type::grep);

  return basic? basic_rep_escapes<CharT,Traits>(t) : 
    erep_escapes<CharT,Traits>(t);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> expression_specials(
    regex_constants::syntax_option_type syntax, const Traits& t) {
  using flag_type = regex_constants::syntax_option_type;
  if (syntax & flag_type::basic) {
    return basic_specials<CharT,Traits>(t);
  }
  if (syntax & flag_type::extended) {
    return extended_specials<CharT,Traits>(t);
  }
  if (syntax & flag_type::ECMAScript) {
    return ECMAScript_specials<CharT,Traits>(t);
  }

  if (syntax & flag_type::grep) {
    auto table = basic_specials<CharT,Traits>(t);
    table.insert(CharT('\n'), token_type::ALTERNATION);
    return table;
  }
  if (syntax & flag_type::egrep) {
    auto table = extended_specials<CharT,Traits>(t);
    table.insert(CharT('\n'), token_type::ALTERNATION);
    return table;
  }

  return basic_specials<CharT,Traits>(t);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> expression_escapes(
    regex_constants::syntax_option_type syntax, const Traits& t) {
  using flag_type = regex_constants::syntax_option_type;
  if (syntax & flag_type::basic) {
    return basic_escapes<CharT,Traits>(t);
  }
  if (syntax & flag_type::extended) {
    return extended_escapes<CharT,Traits>(t);
  }
  if (syntax & flag_type::ECMAScript) {
    return ECMAScript_escapes<CharT,Traits>(t);
  }

  if (syntax & flag_type::grep) {
    return basic_escapes<CharT,Traits>(t);
  }
  if (syntax & flag_type::egrep) {
    return extended_escapes<CharT,Traits>(t);
  }

  return basic_escapes<CharT,Traits>(t);
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> escapes_table(
    regex_constants::context_type con,
    regex_constants::syntax_option_type syntax, const Traits& t) {
  switch (con) {
  case regex_constants::expression:
    return expression_escapes<CharT,Traits>(syntax, t);
  case regex_constants::brace:
    return brace_escapes<CharT,Traits>(syntax, t);
  case regex_constants::bracket:
    return bracket_escapes<CharT,Traits>(syntax, t);
  case regex_constants::subexpression:
    return subexpression_escapes<CharT,Traits>(syntax, t);
  default:
    return expression_escapes<CharT,Traits>(syntax, t);
  }
}

template <typename CharT, typename Traits>
markup_table<CharT, token_type> specials_table(
    regex_constants::context_type con, 
    regex_constants::syntax_option_type syntax, const Traits& t) {
  switch (con) {
  case regex_constants::expression:
    return expression_specials<CharT,Traits>(syntax, t);
  case regex_constants::brace:
    return brace_specials<CharT,Traits>(syntax, t);
  case regex_constants::bracket:
    return bracket_specials<CharT,Traits>(syntax, t);
  case regex_constants::subexpression:
    return subexpression_specials<CharT,Traits>(syntax, t);
  default:
    return expression_specials<CharT,Traits>(syntax, t);
  }
}

}//namespace lex
#endif// _character_markup_tables_h_
