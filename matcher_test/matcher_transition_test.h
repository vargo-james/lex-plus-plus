/* This file tests the matcher_transition class.*/

#ifndef _matcher_transition_test_h_
#define _matcher_transition_test_h_

#include "matcher/matcher_transition.h"
#include "ttest/ttest.h"

int matcher_trans_test();

inline ttest::test_suite::pointer create_matcher_transition_test() {
  using namespace ttest;

  return create_test("matcher_transition", matcher_trans_test);
}

#endif// _matcher_transition_test_h_
