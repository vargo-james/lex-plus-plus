#ifndef _regex_context_test_h_
#define _regex_context_test_h_

#include "ttest/ttest.h"

void regex_context_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_context_test() {
  return create_test("regex_context", regex_context_test);
}

#endif// _regex_context_test_h_
