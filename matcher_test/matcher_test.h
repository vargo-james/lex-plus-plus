/* This file tests the matcher class.
 */

#ifndef _matcher_test_h_
#define _matcher_test_h_

#include "matcher/matcher.h"

#include <ostream>

// This function is the composition of all the individual tests listed
// below it.
int matcher_test(std::ostream& os);

// This tests whether a default constructed matcher will correctly 
// reinitialize itself when its initialize method is called.
int matcher_initialize_test(std::ostream& os);

// This tests whether the default constructed matcher has the correct
// state upon construction, after being updated once. And it does the same
// test for a constructor copy constructed from a default constructed object.
int matcher_constructor_test(std::ostream& os);

#endif// _matcher_test_h_
