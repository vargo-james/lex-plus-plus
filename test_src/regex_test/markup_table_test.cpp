#include "regex/markup_table.h"
#include "regex/regex_token.h"
#include "ttest/ttest.h"

using namespace lex;

markup_table<char, token_type> basic_specials() {
  return markup_table<char,token_type>({
    {'.', token_type::CHAR_CLASS},
    {'[', token_type::L_BRACKET},
    {'*', token_type::REPLICATION},
    {'^', token_type::ASSERTION},
    {'$', token_type::ASSERTION},
    {'\\', token_type::TRAILING_ESCAPE}
  }, token_type::LITERAL);
}

void markup_table_test(ttest::error_log& log) {
  auto table = basic_specials();

  auto mark = table.lookup('*');
  if (*mark != token_type::REPLICATION) {
    log.append("* incorrect mark");
  }
  mark = table.lookup('5');
  if (*mark != token_type::LITERAL) {
    log.append("5 found");
  }
}

ttest::test_suite::pointer create_markup_table_test() {
  using ttest::create_test;

  return create_test("markup_table", markup_table_test);
}
