#ifndef _atomic_test_h_
#define _atomic_test_h_

#include "matcher/atomic.h"
#include "ttest/ttest.h"

// This combines all the tests of the function and classes in the 
// atomic.h header.
//int atomic_test(std::ostream& os);

int singleton_matcher_transition_test();
int singleton_matcher_test();
int predicate_matcher_test();
int universal_singleton_matcher_test();

inline ttest::test_suite::pointer create_atomic_test() {
  using namespace ttest;

  return create_test("atomic", {
    create_test("singleton_matcher_transition", 
        singleton_matcher_transition_test),
    create_test("singleton_matcher", singleton_matcher_test),
    create_test("predicate_matcher", predicate_matcher_test),
    create_test("universal_singleton matcher", 
        universal_singleton_matcher_test),
  });
}

#endif// _atomic_test_h_
