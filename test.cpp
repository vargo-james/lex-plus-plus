/*
 * In this file, we run all the tests.
 */
#include "matcher_test/matcher_module_test.h"

#include <iostream>

int main() {
  int error_count = 0;

  error_count += matcher_module_test();

  if (error_count > 0) {
    std::cerr << error_count << " errors detected.\n";
  }
  else {
    std::cerr << "No errors detected.\n";
  }

  return 0;
}
