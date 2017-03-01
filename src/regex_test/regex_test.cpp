#include "src/matcher_test/matcher_test.h"
#include "regex/regex.h"
#include "ttest/ttest.h"

#include "bracket_test.h"
#include "bracket_list_test.h"

#include <regex>
#include <string>

#include <iostream>

using namespace lex;

void size_display(ttest::error_log& log) {
  std::cout << "Size Display\n";
  std::cout << "size of regex_traits<char>: " << 
    sizeof(std::regex_traits<char>{}) <<
    '\n';
  std::cout << "size of size_t: " << 
    sizeof(size_t) <<
    '\n';
  std::cout << "size of void*: " << 
    sizeof(void*) <<
    '\n';
  std::cout << "size of error_type: " << 
    sizeof(regex_constants::error_type) <<
    '\n';
  std::cout << "size of matcher<char,std::regex_traits<char>>: " << 
    sizeof(matcher<char, std::regex_traits<char>>) <<
    '\n';
  std::cout << "size of std::string: " << 
    sizeof(std::basic_string<char>) <<
    '\n';
  std::cout << "size of std::vector: " << 
    sizeof(std::vector<int>) <<
    '\n';
  std::cout << "size of character_source " << 
    sizeof(character_source<char*, std::regex_traits<char>>) <<
    '\n';
  std::cout << "size of simple_buffer<char> " << 
    sizeof(simple_buffer<char>) <<
    '\n';
  std::cout << "size of token_source " << 
    sizeof(token_source<character_source<char*, std::regex_traits<char>>>) <<
    '\n';
  std::cout << "size of std::cout " << 
    sizeof(std::cout) <<
    '\n';
  std::cout << "size of character_interpreter " <<
    sizeof(character_interpreter<character_source<char*, 
        std::regex_traits<char>>>) << '\n';
  std::cout << "size of markup_table " <<
    sizeof(markup_table<char, token_type>) << '\n';
  std::cout << "size of std::function<bool(char)> " <<
    sizeof(std::function<bool(char)>) << '\n';

  std::cout << "size of bool (*func)(char)> " <<
    sizeof(bool (*)(char)) << '\n';

  std::cout << "size of optional<token_type> " <<
    sizeof(optional<token_type>) << '\n';




  std::cout << '\n';
}

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

  regex<char> reg2("x{5}");
  test_string = "xxxxxx";
  b = test_string.begin();
  it = reg2.match(b, test_string.end());
  if (it != b + 5) {
    log.append("basic repeat");
  }

  regex<char> reg3("\\w{3,}", regex_constants::ECMAScript);
  test_string = "ab_32 ";
  b = test_string.begin();
  it = reg3.match(b, test_string.end());
  if (it != b + 5) {
    log.append("char class");
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

ttest::test_suite::pointer create_regex_test() {
  using ttest::create_test;
  return create_test("regex module", {
      create_test("Size Display\n", size_display),
      create_test("regex::match", regex_match_test),
      create_test("regex", regex_test)
  });
}
