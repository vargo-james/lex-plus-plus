/* This file tests the matcher class.
 */

#ifndef _matcher_test_h_
#define _matcher_test_h_

#include "matcher/matcher.h"
#include "ttest/ttest.h"

#include <string>
#include <vector>


// This tests whether a default constructed matcher will correctly 
// reinitialize itself when its initialize method is called.
int matcher_initialize_test();

// This tests whether the default constructed matcher has the correct
// state upon construction, after being updated once. And it does the same
// test for a constructor copy constructed from a default constructed object.
int matcher_constructor_test();

inline int matcher_fake_test(ttest::error_log& log) {
  log.append("ERROR");
  return 1;
}

inline ttest::test_suite::pointer create_matcher_test() {
  using namespace ttest;

  return create_test("matcher test", {
    create_test("matcher initialize", matcher_initialize_test),
    create_test("matcher constructor", matcher_constructor_test),
    create_test("fake", matcher_fake_test)
  });
}


// This test successively updates a matcher using a string of chars.
// After each update it checks the state against a provided list of states.
int matcher_compare(lex::matcher<char>& matcher, const std::string& updates, 
    const std::vector<lex::match_state>& states, 
    lex::match_state initial = lex::match_state::UNDECIDED);

#endif// _matcher_test_h_
