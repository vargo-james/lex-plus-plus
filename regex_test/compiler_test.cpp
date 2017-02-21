#include "regex/character_source.h"
#include "regex/compiler.h"
#include "matcher_test/matcher_test.h"
#include "ttest/ttest.h"

#include <regex>
#include <string>
#include <utility>
#include <vector>

using namespace lex;

using std::basic_string;
using std::string;
using std::regex_traits;
using std::vector;
using std::pair;

using Iter = typename std::string::const_iterator;
using Traits = std::regex_traits<char>;
using Source = character_source<Iter,Traits>;
using Reader = compiler<Source>;
static Traits t;

template <typename CharT>
char to_char(CharT ch) {
  ch = (ch > 127)? '@' : ch;
  return static_cast<char>(ch);
}
template <typename String>
string to_narrow_string(const String& str) {
  string out;
  for (auto ch : str) {
    out.push_back(to_char(ch));
  }
  return out;
}

template <typename CharT>
void test_matcher(ttest::error_log& log, const basic_string<CharT>& pattern,
    regex_constants::syntax_option_type syntax, 
    const basic_string<CharT>& match,
    const vector<match_state>& expected_states) {
  regex_traits<CharT> traits;
  auto char_source = make_character_source(pattern, traits);
  regex_constants::error_type ec {regex_constants::error_none};

  compiler<decltype(char_source)> reg_compiler(char_source, ec, syntax);
  auto matcher = reg_compiler.compile();

  auto comp = matcher_discrepancies(*matcher, match, expected_states);

  log.append_if("incorrect states: " + to_narrow_string(match), comp);
}

void compiler_literals_test(ttest::error_log& log) {
  test_matcher<char>(log, "a", regex_constants::basic, "ab", {
      match_state::FINAL_MATCH, match_state::MISMATCH,
      });
  test_matcher<char>(log, "abc", regex_constants::basic, "abc&", {
      match_state::UNDECIDED, match_state::UNDECIDED,
      match_state::FINAL_MATCH, match_state::MISMATCH
      });
  test_matcher<char>(log, "abc&()\\[", regex_constants::basic, "abc&()[x", {
      match_state::UNDECIDED, match_state::UNDECIDED,
      match_state::UNDECIDED, match_state::UNDECIDED,
      match_state::UNDECIDED, match_state::UNDECIDED,
      match_state::FINAL_MATCH, match_state::MISMATCH
      });
}

void compiler_alternation_test(ttest::error_log& log) {
  std::string reg {R"(ab\|c)"}; 
  auto ran = make_character_source(reg,t);
  regex_constants::error_type ec;
  Reader compiler(ran, ec, regex_constants::basic);
  auto matcher = compiler.compile();

  auto comp = matcher_discrepancies(*matcher, "abc", {
      match_state::UNDECIDED, match_state::FINAL_MATCH,
      match_state::MISMATCH
      });
  if (!comp) {
    log.append("first");
  }
  matcher->initialize();
  comp = matcher_discrepancies(*matcher, "ca", {
      match_state::MATCH, match_state::MISMATCH
      });
  if (!comp) {
    log.append("second");
  }

  std::string reg2 {"ab\na"};
  auto ran2 = make_character_source(reg2,t);
  regex_constants::error_type ec2;
  Reader compiler2(ran2, ec2, regex_constants::grep);
  matcher = compiler2.compile();

  comp = matcher_discrepancies(*matcher, "ax", {
      match_state::MATCH, match_state::MISMATCH
      });
  if (!comp) {
    log.append("grep first");
  }
  matcher->initialize();
  comp = matcher_discrepancies(*matcher, "ab", {
      match_state::MATCH, match_state::FINAL_MATCH
      });
  if (!comp) {
    log.append("grep second");
  }
}


ttest::test_suite::pointer create_compiler_test() {
  using namespace ttest;
  return create_test("compiler", {
      //create_test("literals", compiler_literals_test),
      //create_test("alternation", compiler_alternation_test),
      });
}
