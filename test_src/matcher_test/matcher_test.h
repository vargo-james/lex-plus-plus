/* This file tests the matcher class.
 */

#ifndef _matcher_test_h_
#define _matcher_test_h_

#include "matcher/matcher.h"

#include <string>
#include <vector>


// This test successively updates a matcher using a string of chars.
// After each update it checks the state against a provided list of states.
// A successful comparison will yield a value of 0.
int matcher_discrepancies(lex::matcher<char>& matcher, const std::string& updates, 
    const std::vector<lex::match_state>& states, 
    lex::match_state initial = lex::match_state::UNDECIDED);

#endif// _matcher_test_h_
