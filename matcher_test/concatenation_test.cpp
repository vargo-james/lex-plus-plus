#include "matcher_test/concatenation_test.h"
#include "matcher_test/matcher_test.h"
#include "matcher/atomic.h"
#include "matcher/string_literal.h"
#include "matcher/concatenation.h"

#include <string>
#include <vector>

void concatenation_test(ttest::error_log& log) {
  using namespace lex;
  using std::move;

  auto str1_matcher = string_matcher<std::string>("ab");
  auto str2_matcher = string_matcher<std::string>("xy");

  std::vector<decltype(str1_matcher)> list {str1_matcher, str2_matcher};

  auto matcher = concatenate(move(list));

  if (matcher_compare(matcher, "abxyz", {
        match_state::UNDECIDED, match_state::UNDECIDED, 
        match_state::UNDECIDED, match_state::FINAL_MATCH, 
        match_state::MISMATCH 
      })) {
    log.append("abxyz");
  }
  matcher.initialize();
}

