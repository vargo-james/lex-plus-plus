#include "matcher_test.h"
#include "matcher/string_literal.h"
#include "matcher/atomic.h"
#include "matcher/replication.h"
#include "ttest/ttest.h"

#include <string>
#include <utility>

static std::regex_traits<char> traits;
using Traits = std::regex_traits<char>;
using namespace std::literals::string_literals;

void char_replication_test(ttest::error_log& log) {
  using namespace lex;

  auto char_match = singleton_matcher('x', traits);
  replication_data repl(1,3);

  auto matcher = replicate(std::move(char_match), repl);
  
  if (matcher_discrepancies(matcher, "xxC;"s, {
        match_state::MATCH, match_state::MATCH, 
        match_state::MISMATCH, match_state::MISMATCH
      })) {
    log.append("xxC");
  }
  matcher.initialize();

  if (matcher_discrepancies(matcher, "xxx."s, {
        match_state::MATCH, match_state::MATCH, 
        match_state::FINAL_MATCH, match_state::MISMATCH
      })) {
    log.append("xxx.");
  }
  matcher.initialize();

  if (matcher_discrepancies(matcher, "ab"s, {
        match_state::MISMATCH, match_state::MISMATCH
      })) {
    log.append("ab");
  }
  matcher.initialize();

  auto alt_char_match = singleton_matcher('.', traits);
  replication_data alt_repl(0, -1);
  auto alt_matcher = replicate(std::move(alt_char_match), alt_repl);

  if (matcher_discrepancies(alt_matcher, ".....x"s, {
        match_state::MATCH, match_state::MATCH, 
        match_state::MATCH, match_state::MATCH,
        match_state::MATCH, match_state::MISMATCH
      }, match_state::MATCH)) {
    log.append(".....x");
  }
  matcher.initialize();
}

void string_replication_test(ttest::error_log& log) {
  using namespace lex;
  using std::move;

  auto str_match = string_matcher<std::string>("ab", traits);
  replication_data repl(1,3);
  auto matcher = replicate(move(str_match), repl);

  if (matcher_discrepancies(matcher, "ababac"s, {
        match_state::UNDECIDED, match_state::MATCH, 
        match_state::UNDECIDED, match_state::MATCH,
        match_state::UNDECIDED, match_state::MISMATCH
      })) {
    log.append("ababac");
  }
  matcher.initialize();

  if (matcher_discrepancies(matcher, "abababc"s, {
        match_state::UNDECIDED, match_state::MATCH, 
        match_state::UNDECIDED, match_state::MATCH,
        match_state::UNDECIDED, match_state::FINAL_MATCH,
        match_state::MISMATCH
      })) {
    log.append("abababc");
  }
  matcher.initialize();
}

void universal_replication_test(ttest::error_log& log) {
  using namespace lex;
  using std::move;
  auto universal = universal_singleton_matcher<char,Traits>();
  replication_data repl(1,3);
  auto matcher = replicate(move(universal), repl);

  if (matcher_discrepancies(matcher, "ab5&"s, {
        match_state::MATCH, match_state::MATCH, 
        match_state::FINAL_MATCH, match_state::MISMATCH 
      })) {
    log.append("ab5&");
  }
  matcher.initialize();
}

void repeated_replication_test(ttest::error_log& log) {
  using namespace lex;
  using std::move;
  auto single_char = singleton_matcher('y', traits);
  replication_data repl1(1,2);
  replication_data repl2(2,3);
  auto matcher1 = replicate(move(single_char), repl1);
  auto matcher = replicate(move(matcher1), repl2);
  
  if (matcher_discrepancies(matcher, "yyyyyyy"s, {
        match_state::UNDECIDED, match_state::MATCH, 
        match_state::MATCH, match_state::MATCH,
        match_state::MATCH, match_state::FINAL_MATCH,
        match_state::MISMATCH
      })) {
    log.append("yyyyyyy");
  }
  matcher.initialize();
}

ttest::test_suite::pointer create_replication_test() {
  using ttest::create_test;

  return create_test("replication", {
      create_test("char", char_replication_test),
      create_test("string", string_replication_test),
      create_test("universal", universal_replication_test),
      create_test("repeated", repeated_replication_test)
  });
}
