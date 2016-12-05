/* 
 * In this file, we implement all the tests of the basic matcher class.
 */

#include "matcher_test/matcher_test.h"
#include "test_machinery.h"

#include <cassert>

using namespace lex;

int matcher_compare(matcher<char>& matcher, const std::string& updates, 
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
}

const std::string match_test {"matcher_test: "};

int default_matcher_test(matcher<char>& mat) {
  return matcher_compare(mat, "a", {match_state::MISMATCH}, 
      match_state::FINAL_MATCH);
}

int matcher_constructor_test() {
  int error_count {0};

  // Default construction is tested here.
  matcher<char> mat;
  error_count += default_matcher_test(mat);
  // Copy construction is tested here.
  matcher<char> mat1;
  auto mat2 = mat1;

  return error_count;
}

// In this test, we do a basic test of the initialize method.
int matcher_initialize_test() {
  int error_count {0};

  matcher<char> mat;
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

