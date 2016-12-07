#include "matcher_test/matcher_test.h"
#include "matcher_test/matcher_replication_test.h"
#include "matcher/atomic_matcher.h"
#include "matcher/matcher_replication.h"

#include <utility>

void char_replication_test(ttest::error_log& log) {
  using namespace lex;

  auto char_match = singleton_matcher('x');
  replication repl(1,3);

  auto matcher = detail::replicate(std::move(char_match), repl);
  
  if (matcher_compare(matcher, "xxC;", {
        match_state::MATCH, match_state::MATCH, 
        match_state::MISMATCH, match_state::MISMATCH
      })) {
    log.append("xxC");
  }
  matcher.initialize();

  if (matcher_compare(matcher, "xxx.", {
        match_state::MATCH, match_state::MATCH, 
        match_state::FINAL_MATCH, match_state::MISMATCH
      })) {
    log.append("xxx.");
  }
  matcher.initialize();

  if (matcher_compare(matcher, "ab", {
        match_state::MISMATCH, match_state::MISMATCH
      })) {
    log.append("ab");
  }
  matcher.initialize();

  auto alt_char_match = singleton_matcher('.');
  replication alt_repl(0, -1);
  auto alt_matcher = detail::replicate(std::move(alt_char_match), alt_repl);

  if (matcher_compare(alt_matcher, ".....x", {
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

  auto str_match = string_matcher<std::string>("ab");
  replication repl(1,3);
  auto matcher = detail::replicate(move(str_match), repl);

  if (matcher_compare(matcher, "ababac", {
        match_state::UNDECIDED, match_state::MATCH, 
        match_state::UNDECIDED, match_state::MATCH,
        match_state::UNDECIDED, match_state::MISMATCH
      })) {
    log.append("ababac");
  }
  matcher.initialize();

  if (matcher_compare(matcher, "abababc", {
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
  auto universal = universal_singleton_matcher<char>();
  replication repl(1,3);
  auto matcher = detail::replicate(move(universal), repl);

  if (matcher_compare(matcher, "ab5&", {
        match_state::MATCH, match_state::MATCH, 
        match_state::FINAL_MATCH, match_state::MISMATCH 
      })) {
    log.append("ab5&");
  }
  matcher.initialize();
}

void repeated_replication_test(ttest::error_log& log) {
  using namespace lex;
  using namespace detail;
  using std::move;
  auto single_char = singleton_matcher('y');
  replication repl1(1,2);
  replication repl2(2,3);
  auto matcher1 = replicate(move(single_char), repl1);
  auto matcher = replicate(move(matcher1), repl2);
  
  if (matcher_compare(matcher, "yyyyyyy", {
        match_state::UNDECIDED, match_state::MATCH, 
        match_state::MATCH, match_state::MATCH,
        match_state::MATCH, match_state::FINAL_MATCH,
        match_state::MISMATCH
      })) {
    log.append("yyyyyyy");
  }
  matcher.initialize();
}
