#ifndef _regex_test_h_
#define _regex_test_h_

#include "ttest/ttest.h"

#include "bracket_test.h"
#include "bracket_list_test.h"
#include "range_test.h"

void regex_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_regex_test() {
  using namespace ttest;
  return create_test("regex module", {
      create_range_test(),
      create_bracket_list_test(),
      create_bracket_test(), 
      create_test("regex", regex_test)
  });
}
#endif// _regex_test_h_
