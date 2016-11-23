/*
 * In this file, the test functions for the atomic_matcher header are defined.
 */

#include "atomic_matcher_test.h"
#include "test_machinery.h"

#include <string>

using namespace lex;


// A singleton_matcher_transition object does not need to change its state
// or even carry any state other than the constant char with which it was
// initialized. 
int singleton_matcher_transition_test(std::ostream& os) {
  int error_count {0};

  std::string smtt {"single_matcher_transition_test: "};

  singleton_matcher_transition<char> trans('a');
  if (trans.update('a') != match_state::FINAL_MATCH) {
    test_log(os, smtt);
    ++error_count;
  }

  if (trans.update('a') != match_state::FINAL_MATCH) {
    test_log(os, smtt);
    ++error_count;
  }

  if (trans.initialize() != match_state::UNDECIDED) {
    test_log(os, smtt);
    ++error_count;
  }

  if (trans.update('x') != match_state::MISMATCH) {
    test_log(os, smtt);
    ++error_count;
  }

  return error_count;
}

int atomic_matcher_test(std::ostream& os) {
  test_suite composition {
    singleton_matcher_transition_test
  };
  return composition(os);
}
