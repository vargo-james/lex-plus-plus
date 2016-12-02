/*
 * In this file, the test functions for the atomic_matcher 
 * header are defined.
 */

#include "atomic_matcher_test.h"
#include "test_machinery.h"

#include <string>

using namespace lex;


// A singleton_matcher_transition object does not need to change its state
// or even carry any state other than the constant char with which it was
// initialized. 
//
// This also tests a copy.
int singleton_matcher_transition_test(std::ostream& os) {
  int error_count {0};
  const std::string smtt {"single_matcher_transition_test: "};

  singleton_matcher_transition<char> trans('a');
  auto copy = trans;
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

  if (copy.update('a') != match_state::FINAL_MATCH) {
    test_log(os, smtt);
    ++error_count;
  }

  if (copy.initialize() != match_state::UNDECIDED) {
    test_log(os, smtt);
    ++error_count;
  }

  if (copy.update('x') != match_state::MISMATCH) {
    test_log(os, smtt);
    ++error_count;
  }
  return error_count;
}

int singleton_matcher_test(std::ostream& os) {
  int error_count {0};
  const std::string smt {"single_matcher_test: "};

  auto matcher = singleton_matcher('b');
  if (matcher.state() != match_state::UNDECIDED) {
    test_log(os, smt);
    ++error_count;
  }
  matcher.update('b');
  if (matcher.state() != match_state::FINAL_MATCH) {
    test_log(os, smt);
    ++error_count;
  }
  matcher.update('b');
  if (matcher.state() != match_state::MISMATCH) {
    test_log(os, smt);
    ++error_count;
  }
  matcher.initialize();
  if (matcher.state() != match_state::UNDECIDED) {
    test_log(os, smt);
    ++error_count;
  }
  matcher.update(';');
  if (matcher.state() != match_state::MISMATCH) {
    test_log(os, smt);
    ++error_count;
  }

  return error_count;
}

int predicate_matcher_test(std::ostream& os) {
  int error_count {0};
  const std::string usmt {"predicate_matcher_test: "};

  auto matcher = 
    predicate_matcher<char>([](const char& ch){return islower(ch);});
  if (matcher.state() != match_state::UNDECIDED) {
    test_log(os, usmt);
    ++error_count;
  }
  matcher.update('r');
  if (matcher.state() != match_state::FINAL_MATCH) {
    test_log(os, usmt);
    ++error_count;
  }
  matcher.update('b');
  if (matcher.state() != match_state::MISMATCH) {
    test_log(os, usmt);
    ++error_count;
  }
  matcher.initialize();
  if (matcher.state() != match_state::UNDECIDED) {
    test_log(os, usmt);
    ++error_count;
  }
  matcher.update(';');
  if (matcher.state() != match_state::MISMATCH) {
    test_log(os, usmt);
    ++error_count;
  }

  return error_count;
}

int universal_singleton_matcher_test(std::ostream& os) {
  int error_count {0};
  const std::string usmt {"universal_singleton_matcher_test: "};

  auto matcher = universal_singleton_matcher<char>();
  if (matcher.state() != match_state::UNDECIDED) {
    test_log(os, usmt);
    ++error_count;
  }
  matcher.update('r');
  if (matcher.state() != match_state::FINAL_MATCH) {
    test_log(os, usmt);
    ++error_count;
  }
  matcher.update('b');
  if (matcher.state() != match_state::MISMATCH) {
    test_log(os, usmt);
    ++error_count;
  }
  matcher.initialize();
  if (matcher.state() != match_state::UNDECIDED) {
    test_log(os, usmt);
    ++error_count;
  }
  matcher.update(';');
  if (matcher.state() != match_state::FINAL_MATCH) {
    test_log(os, usmt);
    ++error_count;
  }

  return error_count;
}

/*
int atomic_matcher_test(std::ostream& os) {
  test_suite composition("atomic matcher test", {
    singleton_matcher_transition_test,
    singleton_matcher_test,
    predicate_matcher_test,
    universal_singleton_matcher_test
  });
  return composition(os);
}*/
