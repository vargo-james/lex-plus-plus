#ifndef matcher_alternation_test_h_
#define matcher_alternation_test_h_

#include "ttest/ttest.h"

void alternation_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_matcher_alternation_test() {
  using namespace ttest;

  return create_test("alternation", alternation_test);
}

#endif// matcher_alternation_test_h_
