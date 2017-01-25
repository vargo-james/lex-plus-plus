#include "token_table_test.h"
#include "regex/token_table.h"


using namespace lex;

void table_test(ttest::error_log& log) {
  auto table = basic_special_characters<char>();

  if (table.contains('&')) {
    log.append("does not contain");
  }
  if (!table.contains('[')) {
    log.append("contains");
  }

  if (table.value('.') != token_type::ANY_CHAR) {
    log.append("ANY_CHAR");
  }

  if (table.value('U') != token_type::LITERAL) {
    log.append("LITERAL");
  }
}

void extended_table_test(ttest::error_log& log) {
  auto table = extended_special_characters<char>();
  if (table.value('?') != token_type::REPLICATION) {
    log.append("?");
  }
  if (table.value(')') != token_type::R_PAREN) {
    log.append(")");
  }
  if (table.value('U') != token_type::LITERAL) {
    log.append("LITERAL");
  }
}

