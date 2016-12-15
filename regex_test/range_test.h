#ifndef _range_test_h_
#define _range_test_h_

#include "ttest/ttest.h"

void range_constructors_test(ttest::error_log& log);
void range_methods_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_range_test() {
  using namespace ttest;
  return create_test("range", {
    create_test("constructors", range_constructors_test),
    create_test("methods", range_methods_test)
  });
}
#endif// _range_test_h_
