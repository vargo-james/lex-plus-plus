#ifndef _replication_reader_test_h_
#define _replication_reader_test_h_

#include "ttest/ttest.h"

void replication_symbol_test(ttest::error_log& log);
void replication_reader_test(ttest::error_log& log);
void replication_errors_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_replication_reader_test() {
  using namespace ttest;
  return create_test("replication reader", {
      create_test("symbol", replication_symbol_test),
      create_test("braces", replication_reader_test),
      create_test("errors", replication_errors_test)
    });
}

#endif// _replication_reader_test_h_
