/*
 * In this file, we run all the tests.
 */
#include "ttest/ttest.h"

#include <iostream>

ttest::test_suite::pointer create_data_structures_module_test();
ttest::test_suite::pointer create_matcher_module_test();
ttest::test_suite::pointer create_regex_module_test();

int main() {
  using std::cerr;
  using ttest::create_test;


  auto lib_test = create_test("lib", {
      create_data_structures_module_test(),
      create_matcher_module_test(),
      create_regex_module_test()
    });

  lib_test->run_test();
  lib_test->report(std::cerr);

  auto error_count = lib_test->error_count();

  if (error_count > 0) {
    std::cerr << error_count << " errors detected.\n";
  }
  else {
    std::cerr << "No errors detected.\n";
  }

  return 0;
}
