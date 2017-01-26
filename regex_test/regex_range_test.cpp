#include "regex_range_test.h"
#include "regex/regex_range.h"

#include <string>
#include <sstream>
#include <iostream>

using namespace lex;

void regex_range_test(ttest::error_log& log) {
  std::string test {"abcd"};
  using It = typename std::string::const_iterator;
  regex_range<It> range(test.begin(), test.end());
  std::istringstream ss {test};
  char left, right;
  while (!range.empty()) {
    if (!range.get(left)) {
      log.append("get");
    }
    if (!(ss >> right)) {
      log.append("stringstream get");
    }
    if (left != right) {
      log.append("mismatch");
    }
  }
  if (ss >> right) {
    log.append("length mismatch");
  }
  range.putback('x');
  range.putback('y');
  range.get(left);
  range.get(right);
  if (left != 'y' || right != 'x') {
    log.append("putback");
  }
  if (!range.empty()) {
    log.append("empty");
  }
  if (range.get(left)) {
    log.append("empty get");
  }

  range.putback(left);
  range.putback(left);
  // Uncommenting this line should fire an assert.
  // range.putback(left);

}
