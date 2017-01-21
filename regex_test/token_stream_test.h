#ifndef token_stream_test_h_
#define token_stream_test_h_

#include "ttest/ttest.h"

void literals_test(ttest::error_log& log);
void replication_test(ttest::error_log& log);
void bracket_test(ttest::error_log& log);
void subexpr_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_token_stream_test() {
  using namespace ttest;
  return create_test("token_stream", {
      create_test("literals", literals_test),
      create_test("replication", replication_test),
      create_test("bracket", bracket_test),
      create_test("subexpr", subexpr_test),
  });

}

#endif// token_stream_test_h_
