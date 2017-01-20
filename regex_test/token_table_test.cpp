#include "token_table_test.h"
#include "regex/token_table.h"

using namespace lex;

void table_test(ttest::error_log& log) {
  token_table<char> table {
    {'.', token_type::ANY_CHAR},
    {'[', token_type::L_BRACKET},
    {'*', token_type::REPLICATION},
    {'^', token_type::L_ANCHOR},
    {'$', token_type::R_ANCHOR}
  };

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

void ERE_table_test(ttest::error_log& log) {
  auto table = ERE_special_characters<char>();
  if (table.value('?') != token_type::REPLICATION) {
    log.append("?");
  }
  if (table.value('}') != token_type::R_BRACE) {
    log.append("}");
  }
  if (table.value('U') != token_type::LITERAL) {
    log.append("LITERAL");
  }
}

