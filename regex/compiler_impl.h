/*
 * This class reads a regex from a range
 */

#ifndef _compiler_impl_h_
#define _compiler_impl_h_

#include "matcher/atomic.h"
#include "matcher/alternation.h"
#include "matcher/concatenation.h"
#include "matcher/matcher.h"
#include "matcher/replication.h"
#include "matcher/string_literal.h"

#include "bracket_reader.h"
#include "error_tracker.h"
#include "data_structures/optional.h"
#include "regex_types.h"
#include "data_structures/simple_buffer.h"
#include "token_source.h"

#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

namespace lex {

template <typename Source>
class compiler_impl : public regex_constants {
 public:
  using value_type = typename Source::value_type;
  using matcher_type = matcher<value_type>;
  using traits_type = typename Source::traits_type;
  using string_type = typename traits_type::string_type;

  compiler_impl(Source& src, error_type& er, syntax_option_type f)
    : source(src, er, f),
      tracker(er),
      syntax_ (f) {}

  optional<matcher_type> get_alternation();
  optional<matcher_type> get_branch();
  optional<matcher_type> get_expression();
  optional<matcher_type> get_element();
  optional<matcher_type> get_bracket();
  optional<matcher_type> get_subexpression();
  optional<replication_data> get_replication();

 private:
  buffered<token_source<Source>> source;
  error_tracker tracker;
  syntax_option_type syntax_;

  bool eat_token(token_type);

  using matcher_list = std::vector<matcher_type>;
  template <typename Compositor>
  matcher_type compose(matcher_list&& elements, 
      Compositor compositor);
};

template <typename Source>
optional<typename compiler_impl<Source>::matcher_type>
compiler_impl<Source>::get_alternation() {
  using std::forward;
  using std::move;

  if (source.empty()) return {};

  std::vector<matcher_type> branches;
  
  for (auto br = get_branch(); br; br = get_branch()) {
    branches.push_back(move(*br));
    if (!eat_token(token_type::ALTERNATION)) {
      break;
    }
  }

  if (tracker.no_error()) {
    matcher_type matcher = compose(move(branches), [](auto&& v) {
          return lex::alternation(forward<matcher_list>(v));
        });

    return matcher;
  }

  return {};
}

template <typename Source>
optional<typename compiler_impl<Source>::matcher_type>
compiler_impl<Source>::get_branch() {
  using std::forward;
  using std::move;

  if (source.empty()) return {};

  std::vector<matcher_type> expressions;
  for (auto exp = get_expression(); exp; exp = get_expression()) {
    expressions.push_back(*exp);
  }

  if (!tracker.error()) {
    matcher_type matcher = compose(move(expressions), [](auto&& vec) {
          return lex::concatenate(forward<matcher_list>(vec));
        });

    return matcher;
  }

  return {};
}

template <typename Source>
optional<typename compiler_impl<Source>::matcher_type>
compiler_impl<Source>::get_expression() {
  using std::move;
  auto matcher = get_element();
  if (!matcher) return {};

  auto data = get_replication();
  if (tracker.error()) return {};

  if (!data) {
    return matcher;
  }

  return replicate(move(*matcher), *data);
}

template <typename Source>
optional<typename compiler_impl<Source>::matcher_type>
compiler_impl<Source>::get_element() {
  auto token = source.get();
  if (!token) return {};

  switch (token->type) {
  case token_type::CONTROL:
  case token_type::ASSERTION:
  case token_type::BACK_REF:
    tracker.set_error( error_type::error_unsupported);
    return {};

  case token_type::L_PAREN:
    return get_subexpression();

  case token_type::L_BRACKET: 
    return get_bracket();

  case token_type::REP_LOWER:
    tracker.set_error(error_type::error_badrepeat);
    return {};

  case token_type::CHAR_CLASS:
    return character_class_matcher<value_type,traits_type>(token->ch);

  case token_type::STRING_LITERAL:
    return string_matcher(std::move(token->str));
  default:
//  ALTERNATION 
    return {};
  }
}

template <typename Source>
optional<replication_data>
compiler_impl<Source>::get_replication() {
  auto token = source.get();
  if (!token) return {};
  if (token->type != token_type::REP_LOWER) {
    source.putback(*token);
    return {};
  }
  auto next_token = source.get();
  assert(next_token->type == token_type::REP_UPPER);
  return replication_data(token->count, next_token->count);
}

template <typename Source>
optional<typename compiler_impl<Source>::matcher_type> 
compiler_impl<Source>::get_subexpression() {
  auto token = source.get();
  if (!token) { 
    return {};
  }
  if (token->type == token_type::NO_SUBEXP ||
      token->type == token_type::ASSERTION) {
    tracker.set_error(error_type::error_unsupported);
    return {};
  } else {
    source.putback(*token);
  }

  auto matcher = get_alternation();
  if (!matcher) return {};

  if (!eat_token(token_type::R_PAREN)) {
    tracker.set_error(error_type::error_paren);
    return {};
  }

  return matcher;
}

template <typename Source>
optional<typename compiler_impl<Source>::matcher_type> 
compiler_impl<Source>::get_bracket() {
  bracket_reader<Source> brack_reader(source, syntax_);
  return brack_reader.read();
}

template <typename Source>
template <typename Compositor>
typename compiler_impl<Source>::matcher_type
compiler_impl<Source>::compose(std::vector<matcher_type>&& elements,
    Compositor compositor) {
  using std::move;
  // The first alternative probably always indicates an error in the regex.
  if (elements.empty()) return {}; 
  if (elements.size() == 1) {
    return move(elements.front()); 
  }
  return compositor(move(elements));
}
  
template <typename Source>
bool compiler_impl<Source>::eat_token(token_type type) {
  auto token = source.get();
  if (!token) return false;
  if (token->type != type) {
    source.putback(*token);
    return false;
  }
  return true;
}

}//namespace lex
#endif// _compiler_impl_h_
