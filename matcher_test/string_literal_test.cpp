#include "string_literal_test.h"
#include "matcher_test.h"
#include "matcher/string_literal.h"

using namespace lex;


void string_literal_test(ttest::error_log& log) {
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
