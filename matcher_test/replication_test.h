#ifndef _replication_test_h_
#define _replication_test_h_

#include "ttest/ttest.h"

void char_replication_test(ttest::error_log& log);
void string_replication_test(ttest::error_log& log);
void universal_replication_test(ttest::error_log& log);
void repeated_replication_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_replication_test() {
  using namespace ttest;

  return create_test("replication", {
      create_test("char", char_replication_test),
      create_test("string", string_replication_test),
      create_test("universal", universal_replication_test),
      create_test("repeated", repeated_replication_test)
  });
}

#endif// _replication_test_h_
