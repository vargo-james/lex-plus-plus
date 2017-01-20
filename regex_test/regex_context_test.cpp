#include "regex_context_test.h"
#include "regex/regex_context.h"

using namespace lex;

void regex_context_test(ttest::error_log& log) {
  regex_context c;  

  if (c.get() != regex_context::DEFAULT) {
    log.append("default constructor");
  }

  c.update(token_type::L_PAREN);
  if (c.get() != regex_context::SUBEXPR) {
    log.append("subexpr entrance");
  }
  c.update(token_type::R_PAREN);
  if (c.get() != regex_context::DEFAULT) {
    log.append("subexpr exit");
  }
  c.update(token_type::L_BRACKET);
  if (c.get() != regex_context::BRACKET) {
    log.append("bracket entrance");
  }
  c.update(token_type::L_COLLATE);
  if (c.get() != regex_context::COLLATE) {
    log.append("collate entrance");
  }

}
