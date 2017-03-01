#include "src/matcher_test/matcher_test.h"
#include "regex/character_source.h"
#include "regex/compiler_impl.h"
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
void get_element_test(ttest::error_log& log, const basic_string<CharT>& pattern,
    regex_constants::syntax_option_type syntax, 
    const basic_string<CharT>& match,
    const vector<match_state>& expected_states) {
  regex_traits<CharT> traits;
  auto char_source = make_character_source(pattern, traits);
  regex_constants::error_type ec {regex_constants::error_none};

  compiler_impl<decltype(char_source)> compiler(char_source, ec, syntax);
  auto matcher = compiler.get_element();
  log.append_if("error: " + pattern, !matcher);

  auto comp = matcher_discrepancies(*matcher, match, expected_states);
  log.append_if("incorrect states: " + to_narrow_string(match), comp);
}

void compiler_get_element_test(ttest::error_log& log) {
  get_element_test<char>(log, "abc", regex_constants::extended, "abcd", {
      match_state::UNDECIDED, match_state::UNDECIDED,
      match_state::FINAL_MATCH, match_state::MISMATCH
    });
  get_element_test<char>(log, "xy\\u0041", regex_constants::ECMAScript, 
      "xyAp", {
      match_state::UNDECIDED, match_state::UNDECIDED,
      match_state::FINAL_MATCH, match_state::MISMATCH
    });
  get_element_test<char>(log, "r", regex_constants::extended, "r", {
      match_state::FINAL_MATCH
    });
  get_element_test<char>(log, "s", regex_constants::extended, "sb", {
      match_state::FINAL_MATCH, match_state::MISMATCH
    });
}

ttest::test_suite::pointer create_compiler_impl_test() {
  using ttest::create_test;
  return create_test("compiler_impl", {
      create_test("get_element", compiler_get_element_test)
    });
}
