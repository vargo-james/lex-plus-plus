/*
 * In this file, we run all the tests.
 */
#include "matcher_test/matcher_module_test.h"
#include "test_machinery.h"

#include <iostream>


int main() {
  using std::cerr;

  test_suite lib_test {matcher_module_test};

  auto error_count = lib_test(std::cerr);

  if (error_count > 0) {
    std::cerr << error_count << " errors detected.\n";
  }
  else {
    std::cerr << "No errors detected.\n";
  }

  return 0;
}
