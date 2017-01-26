#include "matcher_test/matcher_test.h"
#include "regex_test.h"
#include "regex/regex.h"

#include <iostream>
#include <regex>
#include <string>

using namespace lex;

void regex_match_test(ttest::error_log& log) {
  regex<char> my_reg("abc");

  std::string test_string("abcdabc");
  auto b = test_string.begin();
  auto e = test_string.end();

  auto it = my_reg.match(b, e);
  if (it != b + 3) {
    log.append("first");
  }

  it = my_reg.match(it, e);
  if (it != b + 3) {
    log.append("second");
  }


  ++it;
  it = my_reg.match(it, e);
  if (it != e) {
    log.append("third");
  }
}

void regex_test(ttest::error_log& log) {
  regex<char> reg1(std::string("qwerty"));
  auto reg2 = reg1;

  regex<char> super_reg(std::string("qwerty"), regex_constants::icase);

  regex<char> error({'a', 'b'});
  regex<char> error2({'a', 'b'}, regex_constants::ECMAScript);

  regex<char> reg3(std::initializer_list<char>{'q','w','e','r','t','y'}, 
      regex_constants::ECMAScript);

  reg2.assign(super_reg);

  regex<char> reg4("qwerty", 2);
}

