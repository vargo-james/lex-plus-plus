#ifndef _bracket_test_h_
#define _bracket_test_h_

#include "ttest/ttest.h"

void singleton_bracket_test(ttest::error_log& log);
void close_dash_test(ttest::error_log& log);
void range_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_bracket_test() {
  using namespace ttest;

  return create_test("bracket", {
      create_test("singleton", singleton_bracket_test),
      create_test("close_dash", close_dash_test),
      create_test("range", range_test)
  });
}
#endif// _bracket_test_h_
