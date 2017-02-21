#include "matcher_test.h"
#include "matcher/string_literal.h"
#include "ttest/ttest.h"

#include <utility>

using namespace lex;
using std::string;
using std::vector;

void test_string_literal(ttest::error_log& log, const string& pattern,
    const string& comparison, const vector<match_state>& states) {
  
  auto matcher = string_matcher(std::move(pattern));

  log.append_if(pattern, matcher_compare(matcher, comparison, states));
}

void string_literal_test(ttest::error_log& log) {

  test_string_literal(log, "p", "pc", {
      match_state::FINAL_MATCH, match_state::MISMATCH
    });

  test_string_literal(log, "Exact;", "ExaCt;", {
        match_state::UNDECIDED, match_state::UNDECIDED, 
        match_state::UNDECIDED, match_state::MISMATCH,
        match_state::MISMATCH, match_state::MISMATCH 
      });

  test_string_literal(log, "Exact;", "Exact;;", {
        match_state::UNDECIDED, match_state::UNDECIDED, 
        match_state::UNDECIDED, match_state::UNDECIDED,
        match_state::UNDECIDED, match_state::FINAL_MATCH,
        match_state::MISMATCH 
      });
}

ttest::test_suite::pointer create_string_literal_test() {
  using namespace ttest;
  return create_test("string literal", string_literal_test);
}
