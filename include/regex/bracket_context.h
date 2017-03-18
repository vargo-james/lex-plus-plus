#ifndef _bracket_context_h_
#define _bracket_context_h_

#include "project_assert.h"

namespace lex {

class bracket_context {
 public:
  enum context {
    FIRST_CHAR,
    AFTER_NEGATION,
    ELEMENT,
    ELEMENT_OR_DASH,
    AFTER_DASH,
    CLOSED
  };

  bracket_context() : context_ {FIRST_CHAR} {}

  context get() const {return context_;}
  void update(token_type t);
 private:
  context context_;

  void first_char_update(token_type t);
  void after_dash_update(token_type t);
};

inline void bracket_context::update(token_type t) {
  switch (t) {
  case token_type::NEGATION:
    context_ = AFTER_NEGATION;
    break;
  case token_type::BRACKET_LITERAL:
  case token_type::COLLATE:
    if (context_ == AFTER_DASH) {
      context_ = ELEMENT;
    } else {
      context_ = ELEMENT_OR_DASH;
    }
    break;
  case token_type::EQUIV:
  case token_type::CLASS:
    context_ = ELEMENT;
    break;
  case token_type::RANGE_DASH:
    context_ = UPPER;
    break;
  case token_type::R_BRACKET:
    context_ = CLOSED;
    break;
  default:
    assert(t == token_type::NEGATION);
    break;
  }
}

}//namespace 
#endif// _bracket_context_h_
