#ifndef input_buffer_test_h_
#define input_buffer_test_h_

#include "ttest/ttest.h"

void input_buffer_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_input_buffer_test() {
  using namespace ttest;

  return create_test("input buffer", input_buffer_test); 
}

#endif// input_buffer_test_h_
