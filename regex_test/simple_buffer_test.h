#ifndef _simple_buffer_test_h_
#define _simple_buffer_test_h_

#include "ttest/ttest.h"


void simple_buffer_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_buffer_test() {
  return create_test("simple_buffer", simple_buffer_test);
}

#endif// _simple_buffer_test_h_
