/* 
 * In this file, the test functions of the matcher_transition_class
 * are defined.
 */ 

#include "matcher/matcher_transition.h"
#include "ttest/ttest.h"

using namespace lex;

int matcher_trans_test() {
  matcher_transition<char> trans;

  for (auto ch = 'a'; ch != 'z'; ++ch) {
    if (trans.update(ch) != match_state::MISMATCH) {
      return 1;
    }
  }
  return 0;
}

ttest::test_suite::pointer create_matcher_transition_test() {
  using namespace ttest;

  return create_test("transition", matcher_trans_test);
}
