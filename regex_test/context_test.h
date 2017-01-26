#ifndef _context_test_h_
#define _context_test_h_

#include "ttest/ttest.h"

void context_switching_test(ttest::error_log& log);
void context_depth_test(ttest::error_log& log);
void bracket_context_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_context_test() {
  using namespace ttest;
  return create_test("context", {
      create_test("switching", context_switching_test),
      create_test("depth", context_depth_test),
      create_test("bracket",bracket_context_test)
    });
}

#endif// _context_test_h_
