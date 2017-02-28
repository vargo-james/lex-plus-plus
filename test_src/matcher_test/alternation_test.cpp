#include "matcher_test.h"
#include "matcher/atomic.h"
#include "matcher/string_literal.h"
#include "matcher/alternation.h"
#include "ttest/ttest.h"

#include <string>

static std::regex_traits<char> traits;
using Traits = std::regex_traits<char>;
using namespace std::literals::string_literals;

void alternation_test(ttest::error_log& log) {
  using namespace lex;
  using std::move;
  using std::string;

  auto str1 = string_matcher("abc"s, traits);
  auto str2 = string_matcher("xy"s, traits);

  std::vector<decltype(str1)> list {str1, str2};
  auto matcher = alternation(move(list));

  if (matcher_discrepancies(matcher, "abxyz"s, {
        match_state::UNDECIDED, match_state::UNDECIDED, 
        match_state::MISMATCH, match_state::MISMATCH, 
        match_state::MISMATCH 
      })) {
    log.append("abxyz");
  }
  matcher.initialize();
  if (matcher_discrepancies(matcher, "xyz"s, {
        match_state::UNDECIDED, match_state::FINAL_MATCH, 
        match_state::MISMATCH
      })) {
    log.append("xyz");
  }
  matcher.initialize();
  if (matcher_discrepancies(matcher, "abc"s, {
        match_state::UNDECIDED, match_state::UNDECIDED, 
        match_state::FINAL_MATCH
      })) {
    log.append("xyz");
  }
}

ttest::test_suite::pointer create_alternation_test() {
  using ttest::create_test;

  return create_test("alternation", alternation_test);
}
