#include "reader_test.h"
#include "regex/reader.h"
#include "matcher_test/matcher_test.h"

using namespace lex;

void reader_literals_test(ttest::error_log& log) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  using Reader = reader<Iter,Traits>;

  std::string reg {R"(abc&()\[)"}; 
  regex_range<Iter> ran(reg.begin(), reg.end());
  regex_constants::error_type ec;
  Reader reader(ran, regex_constants::basic, ec);
  auto matcher = reader.read();

  auto comp = matcher_compare(matcher, "abc&()[", {
      match_state::UNDECIDED, match_state::UNDECIDED,
      match_state::UNDECIDED, match_state::UNDECIDED,
      match_state::UNDECIDED, match_state::UNDECIDED,
      match_state::FINAL_MATCH, match_state::MISMATCH
      });

  if (!comp) {
    log.append("basic literals");
  }
}

void reader_alternation_test(ttest::error_log& log) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  using Reader = reader<Iter,Traits>;

  std::string reg {R"(ab\|c)"}; 
  regex_range<Iter> ran(reg.begin(), reg.end());
  regex_constants::error_type ec;
  Reader reader(ran, regex_constants::basic, ec);
  auto matcher = reader.read();

  auto comp = matcher_compare(matcher, "abc", {
      match_state::UNDECIDED, match_state::FINAL_MATCH,
      match_state::MISMATCH
      });
  if (!comp) {
    log.append("first");
  }
  matcher.initialize();
  comp = matcher_compare(matcher, "ca", {
      match_state::MATCH, match_state::MISMATCH
      });
  if (!comp) {
    log.append("second");
  }

  std::string reg2 {"ab\na"};
  regex_range<Iter> ran2(reg2.begin(), reg2.end());
  regex_constants::error_type ec2;
  Reader reader2(ran2, regex_constants::grep, ec2);
  matcher = reader2.read();

  comp = matcher_compare(matcher, "ax", {
      match_state::MATCH, match_state::MISMATCH
      });
  if (!comp) {
    log.append("grep first");
  }
  matcher.initialize();
  comp = matcher_compare(matcher, "ab", {
      match_state::MATCH, match_state::FINAL_MATCH
      });
  if (!comp) {
    log.append("grep second");
  }


}
