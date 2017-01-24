#include "context_test.h"
#include "regex/context.h"

using namespace lex;

void context_test(ttest::error_log& log) {
  context c;  

  if (c.get() != context::DEFAULT) {
    log.append("default constructor");
  }

  c.update(token_type::L_PAREN);
  if (c.get() != context::SUBEXPR) {
    log.append("subexpr entrance");
  }
  c.update(token_type::R_PAREN);
  if (c.get() != context::DEFAULT) {
    log.append("subexpr exit");
  }
  c.update(token_type::L_BRACKET);
  if (c.get() != context::BRACKET) {
    log.append("bracket entrance");
  }
  c.update(token_type::L_COLLATE);
  if (c.get() != context::COLLATE) {
    log.append("collate entrance");
  }

}
