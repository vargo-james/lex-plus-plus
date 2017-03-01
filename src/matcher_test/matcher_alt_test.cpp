//#include "regex/matcher/matcher_alt.h"
#include "regex_types.h"
#include "ttest/ttest.h"

#include <regex>
#include <string>

#include <iostream>

using namespace lex;
/*
void string_matcher_alt_test(ttest::error_log& log) {
  std::regex_traits<char> traits;
  regex_constants::syntax_option_type syntax {regex_constants::ECMAScript};

  match_results<std::string> results;

  string_matcher<char, std::regex_traits<char>> matcher("beluga", syntax, 
      traits);

  std::string test_str1 {"beluga whale"};
  std::string test_str2 {"BeluGa"};

  auto begin = make_forward_iterator(test_str1.cbegin());
  auto end = make_forward_iterator(test_str1.cend());

  auto it = matcher.match(results, begin, end);
  std::cout << "first test: " << results.full_results() << '\n';

  results.clear();
  begin = make_forward_iterator(test_str2.cbegin());
  end = make_forward_iterator(test_str2.cend());
  it = matcher.match(results, begin, end);
  std::cout << "second test: " << results.full_results() << '\n';
}

ttest::test_suite::pointer create_matcher_alt_test() {
  using ttest::create_test;
  return create_test("matcher_alt", {
      create_test("string_matcher", string_matcher_alt_test),
    });
}
*/
