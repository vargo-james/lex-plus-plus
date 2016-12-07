#include "matcher_test/alternation_test.h"
#include "matcher_test/matcher_test.h"
#include "matcher/atomic.h"
#include "matcher/alternation.h"

#include <string>

void alternation_test(ttest::error_log& log) {
  using namespace lex;
  using std::move;
  using std::string;

  auto str1 = string_matcher<string>("abc");
  auto str2 = string_matcher<string>("xy");

  std::vector<decltype(str1)> list {str1, str2};
  auto matcher = alternation(move(list));

  if (matcher_compare(matcher, "abxyz", {
        match_state::UNDECIDED, match_state::UNDECIDED, 
        match_state::MISMATCH, match_state::MISMATCH, 
        match_state::MISMATCH 
      })) {
    log.append("abxyz");
  }
  matcher.initialize();
  if (matcher_compare(matcher, "xyz", {
        match_state::UNDECIDED, match_state::FINAL_MATCH, 
        match_state::MISMATCH
      })) {
    log.append("xyz");
  }
  matcher.initialize();
  if (matcher_compare(matcher, "abc", {
        match_state::UNDECIDED, match_state::UNDECIDED, 
        match_state::FINAL_MATCH
      })) {
    log.append("xyz");
  }
}
