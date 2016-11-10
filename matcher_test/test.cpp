/*
 * In this file, we run all the tests.
 */
#include "match_state_transition_test.h"

#include <iostream>

void report_error(int code) {
  if (code != 0) {
    std::cerr << "Error " << code << '\n';
  }
}

int main() {
  int code = 0;
  int error_count = 0;

  code = test_update();
  error_count += code;

  report_error(code);
  if (error_count > 0) {
    std::cout << error_count << " errors occurred.\n";
  }

  return 0;
}
