/*
 * In this file, we run all the tests.
 */
#include "matcher_test/matcher_module_test.h"
#include "ttest/ttest.h"

#include <iostream>


int main() {
  using std::cerr;

  auto lib_test_ptr = ttest::create_test("library test", {
      create_matcher_module_test()
    });
  lib_test_ptr->run_test();
  lib_test_ptr->report(std::cerr);

  auto error_count = lib_test_ptr->error_count();

  if (error_count > 0) {
    std::cerr << '\n' << error_count << " errors detected.\n";
  }
  else {
    std::cerr << "No errors detected.\n";
  }

  return 0;
}
