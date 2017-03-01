/*
 * In this file, the test functions for the atomic 
 * header are defined.
 */

#include "matcher/atomic.h"
#include "matcher_test.h"
#include "ttest/ttest.h"

#include <string>

static std::regex_traits<char> traits;
using Traits = std::regex_traits<char>;
using namespace std::literals::string_literals;

using namespace lex;


// A singleton_matcher_impl object does not need to change its state
// or even carry any state other than the constant char with which it was
// initialized. 
//
// This also tests a copy.
int singleton_matcher_impl_test() {
  int error_count {0};

  detail::singleton_matcher_impl<char,Traits> trans('a', traits);
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

  auto matcher = singleton_matcher('b', traits);

  error_count += matcher_discrepancies(matcher, "bb"s, 
      {match_state::FINAL_MATCH, match_state::MISMATCH});

  matcher.initialize();

  error_count += matcher_discrepancies(matcher, ";"s, {match_state::MISMATCH});

  return error_count;
}

int predicate_matcher_test() {
  int error_count {0};

  auto matcher = 
    predicate_matcher<char,Traits>([](const char& ch){return islower(ch);});

  error_count += matcher_discrepancies(matcher, "rb"s, 
      {match_state::FINAL_MATCH, match_state::MISMATCH});

  matcher.initialize();
  error_count += matcher_discrepancies(matcher, ";"s, {match_state::MISMATCH});

  return error_count;
}

int universal_singleton_matcher_test() {
  int error_count {0};

  auto matcher = universal_singleton_matcher<char,Traits>();

  error_count += matcher_discrepancies(matcher, "rb"s, 
      {match_state::FINAL_MATCH, match_state::MISMATCH});

  matcher.initialize();

  error_count += matcher_discrepancies(matcher, ";"s, {match_state::FINAL_MATCH});

  return error_count;
}

ttest::test_suite::pointer create_atomic_test() {
  using ttest::create_test;

  return create_test("atomic", {
    create_test("singleton_matcher_impl", 
        singleton_matcher_impl_test),
    create_test("singleton_matcher", singleton_matcher_test),
    create_test("predicate_matcher", predicate_matcher_test),
    create_test("universal_singleton matcher", 
        universal_singleton_matcher_test),
  });
}
