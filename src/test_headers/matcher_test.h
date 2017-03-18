/* This file tests the matcher class.
 */

#ifndef _matcher_test_h_
#define _matcher_test_h_

#include "matcher/matcher.h"

#include "project_assert.h"
#include <regex>
#include <string>
#include <vector>


// This test successively updates a matcher using a string of chars.
// After each update it checks the state against a provided list of states.
// A successful comparison will yield a value of 0.
template <typename CharT, typename Traits = std::regex_traits<CharT>>
int matcher_discrepancies(lex::matcher<CharT, Traits>& matcher, 
    const std::basic_string<CharT>& updates, 
    const std::vector<lex::match_state>& states, 
    lex::match_state initial = lex::match_state::UNDECIDED) {
  // First we check that our test is well formed.
  assert(updates.size() == states.size());
  // Any inconsistency will return 1.
  if (matcher.state() != initial) {
    return 1;
  }
  for (auto i = 0u; i != updates.size(); ++i) {
    matcher.update(updates[i]);
    if (matcher.state() != states[i]) {
      return 1;
    }
  }
  // Return 0 if the updates were consistent with the predictions.
  return 0;
}


#endif// _matcher_test_h_
