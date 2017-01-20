#ifndef _token_table_test_h_
#define _token_table_test_h_

#include "ttest/ttest.h"

void table_test(ttest::error_log& log);

void ERE_table_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_table_test() {
  using ttest::create_test;
  return create_test("token_table", {
      create_test("methods", table_test),
      create_test("ERE", ERE_table_test) 
  });
}

#endif// _token_table_test_h_
