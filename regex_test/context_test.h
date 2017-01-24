#ifndef _context_test_h_
#define _context_test_h_

#include "ttest/ttest.h"

void context_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_context_test() {
  return create_test("context", context_test);
}

#endif// _context_test_h_
