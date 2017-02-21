#ifndef _bracket_interpreter_h_
#define _bracket_interpreter_h_

#include "data_structures/optional.h"
#include "regex_token.h"
#include "regex_types.h"
#include "data_structures/simple_buffer.h"
#include "table_interpreter.h"

#include <cassert>

namespace lex {

template <typename Source>
class bracket_interpreter : public regex_constants {
 public:
  using char_type = typename Source::value_type;
  using traits_type = typename Source::traits_type;
  using value_type = regex_token<char_type, traits_type>;

  bracket_interpreter(Source& src, error_type& ec, syntax_option_type syntax)
    : interpreter(src, ec, syntax, regex_constants::bracket),
      tracker(ec) {}

  bool empty() const {return interpreter.empty();}

  optional<value_type> get();

 private:
  table_interpreter<Source> interpreter;
  error_tracker tracker;
  enum bracket_context {
    first_char, after_negation, element, element_or_dash, after_dash
  } context_ {first_char};
  simple_buffer<value_type> buffer;

  optional<value_type> get_in_context();
  void update_context(token_type type);

  bool check_if_empty();
  optional<value_type> filter_get(token_type typeA, token_type typeB, 
      token_type typeC);
  optional<value_type> filter_get(token_type typeA, token_type typeB) {
    return filter_get(typeA, typeB, typeB);
  }
  optional<value_type> filter_get(token_type type) {
    return filter_get(type,type);
  }
};

template <typename Source>
optional<typename bracket_interpreter<Source>::value_type>
bracket_interpreter<Source>::get() {
  auto token = buffer.empty()? get_in_context() : buffer.get();
  
  if (token->type == token_type::R_BRACKET) {
    return token;
  }

  auto next_token = get_in_context();
  if (!next_token) {
    return token;
  }

  if (next_token->type == token_type::R_BRACKET &&
      token->type == token_type::RANGE_DASH) {
    token->type = token_type::BRACKET_LITERAL;
  }

  buffer.push(*next_token);

  return token;
}


template <typename Source>
optional<typename bracket_interpreter<Source>::value_type>
bracket_interpreter<Source>::get_in_context() {
  optional<value_type> token;
  switch (context_) {
  case first_char:
    token = filter_get(token_type::R_BRACKET, token_type::RANGE_DASH);
    break;
  case after_negation:
    token = filter_get(token_type::NEGATION, token_type::R_BRACKET, 
        token_type::RANGE_DASH);
    break;
  case element:
    token = filter_get(token_type::NEGATION);
    break;
  case element_or_dash:
  case after_dash:
    token = filter_get(token_type::NEGATION);
    break;
  default:
    return {};
  }
  if (!token) return {};
  update_context(token->type);
  return token;
}

template <typename Source>
void bracket_interpreter<Source>::update_context(token_type type) {
  switch (type) {
  case token_type::NEGATION:
    context_ = after_negation;
      break;
  case token_type::RANGE_DASH:
    context_ = after_dash;
    break;
  case token_type::EQUIV:
  case token_type::CLASS:
    context_ = element;
    break;
  case token_type::BRACKET_LITERAL:
  case token_type::COLLATE:
    context_ = (context_ == after_dash)? element : element_or_dash;
    break;
  case token_type::R_BRACKET:
    context_ = first_char;
    break;
  default:
    assert(type == token_type::R_BRACKET);
  }
}

template <typename Source>
optional<typename bracket_interpreter<Source>::value_type>
bracket_interpreter<Source>::filter_get(token_type typeA, token_type typeB, 
    token_type typeC) {
  if (check_if_empty()) return {};

  auto token = interpreter.get();
  if (token->type == typeA || token->type == typeB || token->type == typeC) {
    token->type = token_type::BRACKET_LITERAL;
  }
  return token;
}

template <typename Source>
bool bracket_interpreter<Source>::check_if_empty() {
  if (interpreter.empty()) {
    tracker.set_error(error_type::error_brack);
    return true;
  }
  return false;
}
}//namespace lex
#endif// _bracket_interpreter_h_
