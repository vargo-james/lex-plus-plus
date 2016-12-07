/*
 * In this file, the test functions for the atomic 
 * header are defined.
 */

#include "atomic_test.h"
#include "matcher_test.h"
#include "ttest/ttest.h"

#include <string>

using namespace lex;


// A singleton_matcher_transition object does not need to change its state
// or even carry any state other than the constant char with which it was
// initialized. 
//
// This also tests a copy.
int singleton_matcher_transition_test() {
  int error_count {0};

  detail::singleton_matcher_transition<char> trans('a');
  auto copy = trans;
  if (trans.update('a') != match_state::FINAL_MATCH) {
    ++error_count;
  }

  if (trans.update('a') != match_state::FINAL_MATCH) {
    ++error_count;
  }

  if (trans.initialize() != match_state::UNDECIDED) {
    ++error_count;
  }

  if (trans.update('x') != match_state::MISMATCH) {
    ++error_count;
  }

  if (copy.update('a') != match_state::FINAL_MATCH) {
    ++error_count;
  }

  if (copy.initialize() != match_state::UNDECIDED) {
    ++error_count;
  }

  if (copy.update('x') != match_state::MISMATCH) {
    ++error_count;
  }
  return error_count;
}

int singleton_matcher_test() {
  int error_count {0};

  auto matcher = singleton_matcher('b');

  error_count += matcher_compare(matcher, "bb", 
      {match_state::FINAL_MATCH, match_state::MISMATCH});

  matcher.initialize();

  error_count += matcher_compare(matcher, ";", {match_state::MISMATCH});

  return error_count;
}

int predicate_matcher_test() {
  int error_count {0};

  auto matcher = 
    predicate_matcher<char>([](const char& ch){return islower(ch);});

  error_count += matcher_compare(matcher, "rb", 
      {match_state::FINAL_MATCH, match_state::MISMATCH});

  matcher.initialize();
  error_count += matcher_compare(matcher, ";", {match_state::MISMATCH});

  return error_count;
}

int universal_singleton_matcher_test() {
  int error_count {0};

  auto matcher = universal_singleton_matcher<char>();

  error_count += matcher_compare(matcher, "rb", 
      {match_state::FINAL_MATCH, match_state::MISMATCH});

  matcher.initialize();

  error_count += matcher_compare(matcher, ";", {match_state::FINAL_MATCH});

  return error_count;
}

void string_matcher_test(ttest::error_log& log) {
  auto matcher = string_matcher<std::string>("Exact;");

  if (matcher_compare(matcher, "ExaCt;", {
        match_state::UNDECIDED, match_state::UNDECIDED, 
        match_state::UNDECIDED, match_state::MISMATCH,
        match_state::MISMATCH, match_state::MISMATCH 
      })) {
    log.append("ExaCt;");
  }

  matcher.initialize();
  if (matcher_compare(matcher, "Exact;;", {
        match_state::UNDECIDED, match_state::UNDECIDED, 
        match_state::UNDECIDED, match_state::UNDECIDED,
        match_state::UNDECIDED, match_state::FINAL_MATCH,
        match_state::MISMATCH 
      })) {
    log.append("Exact;;");
  }
}

