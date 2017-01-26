#ifndef _context_h_
#define _context_h_

#include "regex_token.h"

namespace lex {

class context {
 public:
  enum site {
    DEFAULT, REPLICATION, SUBEXPR, BRACKET, COLLATE, EQUIV, CLASS
  };

  context(): context_ {DEFAULT} {}

  site get() const {return context_;}
  void update(token_type t);
  int depth() const {return subexpr_depth;}

  bool first_bracket_char() const {return bracket_first;}
  bool after_bracket_negation() const {return bracket_after_negation;}
 private:
  site context_;
  int subexpr_depth {0};
  bool bracket_first {false};
  bool bracket_after_negation {false};

  void default_update(token_type tok);
  void replication_update(token_type tok);
  void subexpr_update(token_type tok);
  void bracket_update(token_type tok);
  void coll_update(token_type tok);
  void equiv_update(token_type tok);
  void class_update(token_type tok);
};

inline void context::update(token_type tok) {
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

inline void context::default_update(token_type tok) {
  switch (tok) {
  case token_type::L_BRACE:
    context_ = REPLICATION;
    break;
  case token_type::L_PAREN:
    ++subexpr_depth;
    context_ = SUBEXPR;
    break;
  case token_type::R_PAREN:
    /*
    if (subexpr_depth == 0) {
      break;
    }
    */
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

inline void context::replication_update(token_type tok) {
  if (tok == token_type::R_BRACE) context_ = DEFAULT;
}

inline void context::subexpr_update(token_type tok) {
  context_ = DEFAULT;
  default_update(tok);
}

inline void context::bracket_update(token_type tok) {
  switch (tok) {
  case token_type::NEGATION:
    bracket_after_negation = true;
    bracket_first = false;
    return;
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
  bracket_first = false;
  bracket_after_negation = false;
}

inline void context::coll_update(token_type tok) {
  if (tok == token_type::R_COLLATE) {
    context_ = BRACKET;
  }
}
inline void context::equiv_update(token_type tok) {
  if (tok == token_type::R_EQUIV) {
    context_ = BRACKET;
  }
}
inline void context::class_update(token_type tok) {
  if (tok == token_type::R_CLASS) {
    context_ = BRACKET;
  }
}

}//namespace lex
#endif// _context_h_
