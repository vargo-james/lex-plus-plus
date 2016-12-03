#ifndef _atomic_matcher_test_h_
#define _atomic_matcher_test_h_

#include "matcher/atomic_matcher.h"
#include "test_machinery.h"

#include <ostream>

// This combines all the tests of the function and classes in the 
// atomic_matcher.h header.
//int atomic_matcher_test(std::ostream& os);

int singleton_matcher_transition_test(std::ostream& os);
int singleton_matcher_test(std::ostream& os);
int predicate_matcher_test(std::ostream& os);
int universal_singleton_matcher_test(std::ostream& os);

const test_suite atomic_matcher_test("atomic matcher", {
    test_suite("singleton matcher transition",
        {singleton_matcher_transition_test}),
    test_suite("singleton matcher", {singleton_matcher_test}),
    test_suite("predicate matcher", {predicate_matcher_test}),
    test_suite("universal singleton matcher", {
        universal_singleton_matcher_test})
});

// This tests the singleton_matcher_transition class.
int singleton_matcher_transition_test(std::ostream& os);

#endif// _atomic_matcher_test_h_
