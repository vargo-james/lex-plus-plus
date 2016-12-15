#include "range_test.h"
#include "regex/range.h"
#include "regex_types.h"

#include <string>

#include <iostream>
using namespace lex;

void range_constructors_test(ttest::error_log& log) {
  using std::string;

  string fb {"foobar"};
  using It = typename string::const_iterator;

  range<It> r(fb);

  auto r2(r);

  r.advance();
  if (r.current() == r2.current()) {
    log.append("copy failure");
  }

  r2.advance();
  if (r2.current() != r.current()) {
    log.append("fail");
  }
}

void range_methods_test(ttest::error_log& log) {
  using std::string;
  using It = typename string::const_iterator;

  string s {"foo[]"};
  range<It> r(s);

  try {
    auto r2 = r;

    for (auto i = 0u; i < 5; ++i) {
      r.advance();
    }
    r.check(r.empty(), "not empty");

    r2.check(r2.current() == 'f');
    r2.advance().advance().advance();
    r2.check(r2.bracket_open(), "open");
    r2.advance();
    r2.check(r2.bracket_close(), "close");
    r2.advance();
    r2.check(r2.empty(), "r2 not empty");
  }
  catch(regex_error& e) {
    log.append(e.what());
  }
  try {
    string t{"foo"};
    range<It> ra(t);
    ra.checked_advance().checked_advance().checked_advance();
    log.append("checked_advance");
  }
  catch(regex_error& e) {
  }
}
