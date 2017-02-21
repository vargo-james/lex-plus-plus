#ifndef _token_source_h_
#define _token_source_h_

#include "bracket_interpreter.h"
#include "expression_interpreter.h"
#include "optional.h"
#include "regex_types.h"
#include "regex_token.h"
#include "table_interpreter.h"

#include <cassert>

namespace lex {

template <typename Source>
class token_source : public regex_constants {
 public:
  using char_type = typename Source::value_type;
  using traits_type = typename Source::traits_type;
  using value_type = regex_token<char_type, traits_type>;

  token_source(Source& src, error_type& ec, syntax_option_type syntax)
    : exp_interpreter(src, ec, syntax),
      brack_interpreter(src, ec, syntax),
      subexpr_interpreter(src, ec, syntax, context_type::subexpression) {}    

  bool empty() const;
  optional<value_type> get();
 private:
  expression_interpreter<Source> exp_interpreter;
  bracket_interpreter<Source> brack_interpreter;
  table_interpreter<Source> subexpr_interpreter;
  context_type context_ {context_type::expression};

  void update_context(token_type type);
};

template <typename Source>
bool token_source<Source>::empty() const {
  switch (context_) {
  case context_type::expression:
    return exp_interpreter.empty();
  case context_type::bracket:
    return brack_interpreter.empty();
  case context_type::subexpression:
    return subexpr_interpreter.empty();
  default:
    assert(context_ == context_type::expression);
  }
  return true;
}

template <typename Source>
optional<typename token_source<Source>::value_type>
token_source<Source>::get() {
  optional<value_type> token;
  switch (context_) {
  case context_type::expression:
    token = exp_interpreter.get();
    break;
  case context_type::bracket:
    token = brack_interpreter.get();
    break;
  case context_type::subexpression:
    token = subexpr_interpreter.get();
    if (!token) {
      token = exp_interpreter.get();
    }
    break;
  default:
    return {};
  }

  if (!token) return {};
  update_context(token->type);
  return token;
}

template <typename Source>
void token_source<Source>::update_context(token_type type) {
  if (context_ == context_type::subexpression) {
    context_ = context_type::expression;
  }

  switch (type) {
  case token_type::L_PAREN:
    context_ = context_type::subexpression;
    break;
  case token_type::L_BRACKET:
    context_ = context_type::bracket;
    break;
  case token_type::R_BRACKET:
    context_ = context_type::expression;
    break;
  default:
    break;
  }
}

}//namespace lex
#endif// _token_source_h_
