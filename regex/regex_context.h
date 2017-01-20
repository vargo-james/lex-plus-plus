#ifndef _regex_context_h_
#define _regex_context_h_

#include "regex_token.h"

namespace lex {

class regex_context {
 public:
  enum context {
    DEFAULT, REPLICATION, SUBEXPR, BRACKET, COLLATE, EQUIV, CLASS
  };

  regex_context(): context_ {DEFAULT} {}

  context get() const {return context_;}
  void update(token_type t);
 private:
  context context_;

  void default_update(token_type tok);
  void replication_update(token_type tok);
  void subexpr_update(token_type tok);
  void bracket_update(token_type tok);
  void coll_update(token_type tok);
  void equiv_update(token_type tok);
  void class_update(token_type tok);
};

inline void regex_context::update(token_type tok) {
  switch (context_) {
  case DEFAULT:
    default_update(tok);
    break;
  case REPLICATION:
    replication_update(tok);
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
}

inline void regex_context::default_update(token_type tok) {
  switch (tok) {
  case token_type::L_BRACE:
    context_ = REPLICATION;
    break;
  case token_type::L_PAREN:
    context_ = SUBEXPR;
    break;
  case token_type::L_BRACKET:
    context_ = BRACKET;
    break;
  default:
    break;
  }
}

inline void regex_context::replication_update(token_type tok) {
  if (tok == token_type::R_BRACE) context_ = DEFAULT;
}

inline void regex_context::subexpr_update(token_type tok) {
  //if (tok == token_type::R_PAREN) context_ = DEFAULT;
  context_ = DEFAULT;
}

inline void regex_context::bracket_update(token_type tok) {
  switch (tok) {
  case token_type::R_BRACKET:
    context_ = DEFAULT;
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
}

inline void regex_context::coll_update(token_type tok) {
  if (tok == token_type::R_COLLATE) {
    context_ = BRACKET;
  }
}
inline void regex_context::equiv_update(token_type tok) {
  if (tok == token_type::R_EQUIV) {
    context_ = BRACKET;
  }
}
inline void regex_context::class_update(token_type tok) {
  if (tok == token_type::R_CLASS) {
    context_ = BRACKET;
  }
}

}//namespace lex
#endif// _regex_context_h_
