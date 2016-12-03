/* This file tests the matcher_transition class.*/

#ifndef _matcher_transition_test_h_
#define _matcher_transition_test_h_

#include "matcher/matcher_transition.h"
#include "test_machinery.h"

#include <ostream>

int matcher_trans_test(std::ostream& os);

const test_suite matcher_transition_test("matcher transition", 
    {matcher_trans_test});

#endif// _matcher_transition_test_h_
