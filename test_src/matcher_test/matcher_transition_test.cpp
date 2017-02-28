/* 
 * In this file, the test functions of the matcher_impl_class
 * are defined.
 */ 

#include "matcher/matcher_impl.h"
#include "ttest/ttest.h"

#include <regex>

using namespace lex;

int matcher_impl_test() {
  matcher_impl<char, std::regex_traits<char>> trans;

  for (auto ch = 'a'; ch != 'z'; ++ch) {
    if (trans.update(ch) != match_state::MISMATCH) {
      return 1;
    }
  }
  return 0;
}

ttest::test_suite::pointer create_matcher_impl_test() {
  using ttest::create_test;

  return create_test("impl", matcher_impl_test);
}
