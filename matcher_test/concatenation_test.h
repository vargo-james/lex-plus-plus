#ifndef _concatenation_test_h_
#define _concatenation_test_h_

#include "ttest/ttest.h"

void concatenation_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_concatenation_test() {
  using namespace ttest;

  return create_test("concatenation", concatenation_test);
}

#endif// _concatenation_test_h_
