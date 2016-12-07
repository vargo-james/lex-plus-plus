#ifndef alternation_test_h_
#define alternation_test_h_

#include "ttest/ttest.h"

void alternation_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_alternation_test() {
  using namespace ttest;

  return create_test("alternation", alternation_test);
}

#endif// alternation_test_h_
