/* This file tests the matcher_transition class.*/

#ifndef _matcher_transition_test_h_
#define _matcher_transition_test_h_

#include "matcher/matcher_transition.h"
#include "test_machinery.h"

int matcher_trans_test();

inline test_suite::pointer create_matcher_transition_test() {
  return create_test("matcher transition", matcher_trans_test);
}

#endif// _matcher_transition_test_h_
