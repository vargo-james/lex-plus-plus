#ifndef _string_literal_test_h_
#define _string_literal_test_h_

#include "ttest/ttest.h"

void string_literal_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_string_literal_test() {
  using namespace ttest;
  return create_test("string literal", string_literal_test);
}
#endif// _string_literal_test_h_
