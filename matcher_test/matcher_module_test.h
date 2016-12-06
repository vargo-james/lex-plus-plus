#ifndef _matcher_module_test_h_
#define _matcher_module_test_h_

#include "matcher_transition_test.h"
#include "matcher_test.h"
#include "atomic_matcher_test.h"
#include "ttest/ttest.h"

inline ttest::test_suite::pointer create_matcher_module_test() {
  using namespace ttest;

  return create_test("matcher", {
      create_matcher_test()
      , create_matcher_transition_test()
      , create_atomic_matcher_test()
  });
}

#endif// _matcher_module_test_h_
