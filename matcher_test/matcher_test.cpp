/* 
 * In this file, we implement all the tests of the basic matcher class.
 */

#include "matcher_test/matcher_test.h"
#include "test_machinery.h"

using namespace lex;

const std::string match_test {"matcher_test: "};

int default_matcher_test(std::ostream& os, matcher<char>& mat) {
  int error_count {0}; 

  if (mat.state() != match_state::FINAL_MATCH) {
    test_log(os, match_test + "incorrect original state in default"
        " matcher.");
    ++error_count;
  }

  mat.update('a');
  if (mat.state() != match_state::MISMATCH) {
    test_log(os, match_test + "incorrect state in default"
        " matcher after update.");
    ++error_count;
  }
  return error_count;
}

int matcher_constructor_test(std::ostream& os) {
  int error {0};
  int error_count {0};

  // Default construction is tested here.
  matcher<char> mat;
  error = default_matcher_test(os, mat);
  if (error) {
    test_log(os, "default constructor error");
    error_count += error;
  }

  // Copy construction is tested here.
  matcher<char> mat1;
  auto mat2 = mat1;
  error = default_matcher_test(os, mat2);
  if (error) {
    test_log(os, "copy constructor error");
    error_count += error;
  }

  return error_count;
}

int matcher_initialize_test(std::ostream& os) {
  int error {0};
  int error_count {0};

  matcher<char> mat;
  mat.update('a');
  auto mat2 = mat;

  mat.initialize();
  error = default_matcher_test(os, mat);
  if (error) {
    test_log(os, "initialization error");
    error_count += error;
  }

  if (mat2.state() != match_state::MISMATCH) {
    test_log(os, "copy error on updated matcher");
    ++error_count;
  }
  else {
    mat2.initialize();
    error = default_matcher_test(os, mat2);
    if (error) {
      test_log(os, "initialization error on copied object");
      error_count += error;
    }
  }

  return error_count;
}

int matcher_test(std::ostream& os) {
  test_suite composition {
    matcher_constructor_test, 
    matcher_initialize_test
  };
  return composition(os);
}

