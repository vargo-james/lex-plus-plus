#ifndef _atomic_matcher_test_h_
#define _atomic_matcher_test_h_

#include "matcher/atomic_matcher.h"
#include "ttest/ttest.h"

// This combines all the tests of the function and classes in the 
// atomic_matcher.h header.
//int atomic_matcher_test(std::ostream& os);

int singleton_matcher_transition_test();
int singleton_matcher_test();
int predicate_matcher_test();
int universal_singleton_matcher_test();

inline ttest::test_suite::pointer create_atomic_matcher_test() {
  using namespace ttest;

  return create_test("atomic matcher", {
    create_test("singleton_matcher_transition", 
        singleton_matcher_transition_test),
    create_test("singleton matcher", singleton_matcher_test),
    create_test("predicate matcher", predicate_matcher_test),
    create_test("universal singleton matcher", 
        universal_singleton_matcher_test)
  });
}

#endif// _atomic_matcher_test_h_
