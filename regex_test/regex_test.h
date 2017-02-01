#ifndef _regex_test_h_
#define _regex_test_h_

#include "ttest/ttest.h"

#include "bracket_test.h"
#include "bracket_list_test.h"
#include "range_test.h"
#include "regex_range_test.h"
#include "replication_reader_test.h"
#include "context_test.h"
#include "simple_buffer_test.h"
#include "token_stream_test.h"
#include "token_table_test.h"

void regex_match_test(ttest::error_log& log);
void regex_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_regex_test() {
  using namespace ttest;
  return create_test("regex module", {
      create_buffer_test(),
      create_regex_range_test(),
      create_context_test(),
      create_table_test(),
      create_token_stream_test(),
      create_replication_reader_test(),
      create_range_test(),
      create_bracket_list_test(),
      create_bracket_test(), 
      create_test("regex::match", regex_match_test),
      create_test("regex", regex_test)
  });
}
#endif// _regex_test_h_
