#ifndef _bracket_context_h_
#define _bracket_context_h_

namespace lex {

class bracket_context {
 public:
  enum context {FIRST, AFTER_NEGATION, ELEMENT, ELEMENT_OR_RANGE};

};
}//namespace lex
#endif// _bracket_context_h_

