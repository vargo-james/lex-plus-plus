/* 
 * In this file, we implement all the tests of the basic matcher class.
 */

#include "matcher_test.h"
#include "ttest/ttest.h"

#include <cassert>
#include <string>

static std::regex_traits<char> traits;
using Traits = std::regex_traits<char>;
using namespace std::literals::string_literals;

using namespace lex;

/*
int matcher_discrepancies(matcher<char, Traits>& matcher, const std::string& updates, 
    const std::vector<match_state>& states, match_state initial) {
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
}*/

int default_matcher_test(matcher<char,Traits>& mat) {
  return matcher_discrepancies(mat, "a"s, {match_state::MISMATCH}, 
      match_state::FINAL_MATCH);
}

// This tests whether the default constructed matcher has the correct
// state upon construction, after being updated once. And it does the same
// test for a constructor copy constructed from a default constructed object.
int matcher_constructor_test() {
  int error_count {0};

  // Default construction is tested here.
  matcher<char,Traits> mat;
  error_count += default_matcher_test(mat);
  // Copy construction is tested here.
  matcher<char,Traits> mat1;
  auto mat2 = mat1;
  // Copy Assignment is tested here.
  matcher<char,Traits> mat3;
  mat3 = mat2;

  return error_count;
}

// This tests whether a default constructed matcher will correctly 
// reinitialize itself when its initialize method is called.
int matcher_initialize_test() {
  int error_count {0};

  matcher<char,Traits> mat;
  mat.update('a');
  auto mat2 = mat;

  mat.initialize();
  error_count += default_matcher_test(mat);

  if (mat2.state() != match_state::MISMATCH) {
    ++error_count;
  }
  else {
    mat2.initialize();
    error_count += default_matcher_test(mat2);
  }

  return error_count;
}

ttest::test_suite::pointer create_matcher_test() {
  using ttest::create_test;

  return create_test("matcher", {
    create_test("matcher initialize", matcher_initialize_test),
    create_test("matcher constructor", matcher_constructor_test)
  });
}
