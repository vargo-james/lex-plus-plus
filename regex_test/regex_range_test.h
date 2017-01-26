#ifndef _regex_range_test_h_
#define _regex_range_test_h_

#include "ttest/ttest.h"

void regex_range_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_regex_range_test() {
  using namespace ttest;
  return create_test("regex_range", regex_range_test);
}
#endif// _regex_range_test_h_
