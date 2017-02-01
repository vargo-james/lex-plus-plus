/*
 * This class reads a regex from a range
 */

#ifndef _reader_h_
#define _reader_h_

#include "matcher/atomic.h"
#include "matcher/alternation.h"
#include "matcher/concatenation.h"
#include "matcher/matcher.h"
#include "matcher/replication.h"
#include "matcher/string_literal.h"
#include "bracket_reader.h"
#include "replication_reader.h"
#include "token_stream.h"

#include <cassert>
#include <iterator>
#include <regex>
#include <utility>
#include <vector>

namespace lex {

template <typename InputIt, typename Traits>
class reader : public error_tracker, public syntax_option {
 public:
  using error_type = regex_constants::error_type;
  using syntax_option::flag_type;
  using value_type = typename std::iterator_traits<InputIt>::value_type;
  using matcher_type = matcher<value_type>;
  using stream_type = token_stream<InputIt,Traits>;
  using token = typename stream_type::token;

  reader(regex_range<InputIt>& src, flag_type f, error_type& er)
    : error_tracker(er),
      syntax_option(f),
      ts(src, f, er) {}

  matcher_type read();

 private:
  Traits traits_;
  token_stream<InputIt,Traits> ts;

  bool get_alternation(matcher_type& matcher);
  bool get_branch(matcher_type& matcher);
  bool get_expression(matcher_type& matcher);
  bool get_element(matcher_type& matcher);
  bool get_literals(matcher_type& matcher);
  bool get_bracket(matcher_type& matcher);
  bool get_sub_expression(matcher_type& matcher);
  bool get_replication(replication_data& rep);

  template <typename Compositor>
  matcher_type compose(const std::vector<matcher_type>& elements, 
      Compositor compositor);
};

template <typename InputIt, typename Traits>
typename reader<InputIt,Traits>::matcher_type
reader<InputIt,Traits>::read() {
  matcher_type matcher;
  get_alternation(matcher);
  return matcher;
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_alternation(matcher_type& matcher) {
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

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_branch(matcher_type& matcher) {
  using std::move;
  if (ts.empty()) return false;

  std::vector<matcher_type> expressions;
  for (matcher_type exp; get_expression(exp); expressions.push_back(exp)) {}

  matcher = compose(expressions, [](auto&& vec) {
        return lex::concatenate(move(vec));
      });
  return !error();
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_expression(matcher_type& matcher) {
  using std::move;
  if (!get_element(matcher)) return false;

  for (replication_data rep; get_replication(rep);) {
    matcher = replicate(move(matcher), rep);
  }
  return !error();
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_element(matcher_type& matcher) {
  token tok;
  if (!ts.get(tok)) return false;
  switch (tok.type) {
  case token_type::HEX_ESCAPE:
  case token_type::UNICODE_ESCAPE:
  case token_type::CONTROL:
  case token_type::ASSERTION:
  case token_type::BACK_REF:
    set_error( error_type::error_unsupported);
    return false;

  case token_type::L_PAREN:
    return get_sub_expression(matcher);
  case token_type::L_BRACKET:
    return get_bracket(matcher);
  case token_type::L_BRACE:
  case token_type::REPLICATION:
    set_error( error_type::error_badrepeat);
    return false;
  case token_type::ANY_CHAR:
  case token_type::CHAR_CLASS:
    matcher = character_class_matcher<value_type,Traits>(tok.ch);
    return true;
  case token_type::LITERAL:
    ts.putback(tok);
    return get_literals(matcher);
  default:
//  ALTERNATION 
    return false;
  }
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_literals(matcher_type& matcher) {
  using std::move;
  typename Traits::string_type str;
  for (token tok; ts.get(tok); ) {
    if (tok.type != token_type::LITERAL) {
      ts.putback(tok);
      break;
    }
    str.push_back(tok.ch);
  }
  assert(!str.empty());
  matcher = string_matcher(move(str));
  return !error();
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_replication(replication_data& rep) {
  replication_reader<InputIt,Traits> rep_reader(traits_, ts, get_error());
  return rep_reader.read(rep);
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_sub_expression(matcher_type& matcher) {
  token tok;
  if (!ts.get(tok)) { 
    return false;
  }
  if (tok.type == token_type::NO_SUBEXP) {
    set_error(error_type::error_unsupported);
    return false;
  } else {
    ts.putback(tok);
  }

  if (!get_alternation(matcher)) return false;
  // We should be able to read the close parenthesis.
  if (!ts.get(tok)) return false;

  assert(tok.type == token_type::R_PAREN);
  return !error();
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_bracket(matcher_type& matcher) {
  bracket_reader<InputIt,Traits> brack_reader(traits_, ts, flag());
  return brack_reader.read(matcher);
}

template <typename InputIt, typename Traits>
template <typename Compositor>
typename reader<InputIt,Traits>::matcher_type
reader<InputIt,Traits>::compose(const std::vector<matcher_type>& elements,
    Compositor compositor) {
  // The first alternative probably always indicates an error in the regex.
  if (elements.empty()) return {}; 
  if (elements.size() == 1) return elements.front(); 
  return compositor(elements);
}
  

}//namespace lex
#endif// _reader_
