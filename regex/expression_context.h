#ifndef _expression_context_h_
#define _expression_context_h_

#include "error_tracker.h"
#include "regex_token.h"
#include "regex_types.h"
#include "syntax_option.h"

namespace lex {

class expression_context : public error_tracker, public syntax_option {
 public:
  using flag_type = syntax_option::flag_type;
  using error_tracker::error_type;

  enum context {
    EXPRESSION, 
    SUBEXPR, 
    BRACKET, 
    REPLICATION_LOWER, 
    COMMA_OR_R_BRACE, UPPER_OR_R_BRACE, REP_END,
    COLLATE, EQUIV, CLASS
  };

  //expression_context(): context_ {EXPRESSION} {}
  expression_context(error_type& e, flag_type f)
    : error_tracker(e),
      syntax_option(f),
      context_ {EXPRESSION} {}

  context get() const {return context_;}
  bool update(token_type t);
  int depth() const {return subexpr_depth;}

  bool first_bracket_char() const {return bracket_first;}
  bool after_bracket_negation() const {return bracket_after_negation;}

 private:
  context context_;
  int subexpr_depth {0};
  bool bracket_first {false};
  bool bracket_after_negation {false};

  void expression_update(token_type tok);
  void replication_update(token_type tok);
  void subexpr_update(token_type tok);
  void bracket_update(token_type tok);
  void coll_update(token_type tok);
  void equiv_update(token_type tok);
  void class_update(token_type tok);
};

inline bool expression_context::update(token_type tok) {
  switch (context_) {
  case EXPRESSION:
    expression_update(tok);
    break;
  case REPLICATION_LOWER:
    replication_update(tok);
    break;
  case COMMA_OR_R_BRACE:
    switch (tok) {
    case token_type::COUNT_SEP:
      context_ = UPPER_OR_R_BRACE;
      break;
    case token_type::R_BRACE:
      context_ = EXPRESSION;
      break;
    default:
      //set_error(error_type::error_badbrace);
      break;
    }
    break;
  case UPPER_OR_R_BRACE:
    break;
  case SUBEXPR:
    subexpr_update(tok);
    break;
  case BRACKET:
    bracket_update(tok);
    break;
  case COLLATE:
    coll_update(tok);
    break;
  case EQUIV:
    equiv_update(tok);
    break;
  case CLASS:
    class_update(tok);
    break;
  default:
    break;
  }
  return true;
}

inline void expression_context::expression_update(token_type tok) {
  switch (tok) {
  case token_type::L_BRACE:
    context_ = REPLICATION_LOWER;
    break;
  case token_type::L_PAREN:
    ++subexpr_depth;
    if (ECMAScript()) {
      context_ = SUBEXPR;
    }
    break;
  case token_type::R_PAREN:
    --subexpr_depth;
    break;
  case token_type::L_BRACKET:
    bracket_first = true;
    context_ = BRACKET;
    break;
  default:
    break;
  }
}

inline void expression_context::replication_update(token_type tok) {
  if (tok == token_type::R_BRACE) context_ = EXPRESSION;
}

inline void expression_context::subexpr_update(token_type tok) {
  context_ = EXPRESSION;
  expression_update(tok);
}

inline void expression_context::bracket_update(token_type tok) {
  switch (tok) {
  case token_type::NEGATION:
    bracket_after_negation = true;
    bracket_first = false;
    return;
  case token_type::R_BRACKET:
    context_ = EXPRESSION;
    break;
  case token_type::L_COLLATE:
    context_ = COLLATE;
    break;
  case token_type::L_EQUIV:
    context_ = EQUIV;
    break;
  case token_type::L_CLASS:
    context_ = CLASS;
    break;
  default:
    break;
  }
  bracket_first = false;
  bracket_after_negation = false;
}

inline void expression_context::coll_update(token_type tok) {
  if (tok == token_type::R_COLLATE) {
    context_ = BRACKET;
  }
}
inline void expression_context::equiv_update(token_type tok) {
  if (tok == token_type::R_EQUIV) {
    context_ = BRACKET;
  }
}
inline void expression_context::class_update(token_type tok) {
  if (tok == token_type::R_CLASS) {
    context_ = BRACKET;
  }
}

class replication_context {
};

}//namespace lex
#endif// _expression_context_h_
