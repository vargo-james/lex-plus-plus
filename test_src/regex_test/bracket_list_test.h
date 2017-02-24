#ifndef _bracket_list_test_h_
#define _bracket_list_test_h_

#include "ttest/ttest.h"

void matching_bracket_list_test(ttest::error_log& log);
void non_matching_bracket_list_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_bracket_list_test() {
  using namespace ttest;
  return create_test("bracket list", {
      create_test("matching", matching_bracket_list_test),
      create_test("non-matching", non_matching_bracket_list_test)
  });
}
#endif// _bracket_list_test_h_
