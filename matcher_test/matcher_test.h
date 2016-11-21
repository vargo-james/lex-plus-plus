/* This file tests the matcher class.
 */

#include "matcher/matcher.h"

#include <utility>

int matcher_constructor_test() {
  using namespace lex;

  // default construction
  matcher<char> mat;
  // copy construction
  auto mat2 = mat;

  int error_count {0};

  if (mat.state() != match_state::FINAL_MATCH) {
    ++error_count;
  }

  mat.update('a');
  if (mat.state() == mat2.state()) {
    ++error_count;
  }

  matcher<char> mat3;
  mat3 = std::move(mat2);
  if (mat.state() == mat3.state()) {
    ++error_count;
  }

  return error_count;
}

int matcher_initialize_test() {
  return 0;
}

int matcher_test() {
  int error_count {0};

  error_count += matcher_constructor_test();
  error_count += matcher_initialize_test();

  return error_count;
}
