#ifndef _atomic_matcher_test_h_
#define _atomic_matcher_test_h_

#include "matcher/atomic_matcher.h"

#include <ostream>

// This combines all the tests of the function and classes in the 
// atomic_matcher.h header.
int atomic_matcher_test(std::ostream& os);

// This tests the singleton_matcher_transition class.
int singleton_matcher_transition_test(std::ostream& os);

#endif// _atomic_matcher_test_h_
