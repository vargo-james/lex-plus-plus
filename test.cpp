/*
 * In this file, we run all the tests.
 */
#include "matcher_test/matcher_module_test.h"
#include "ttest/ttest.h"

#include <iostream>


int main() {
  using std::cerr;
  using namespace ttest;

  auto lib_test = create_test("lib", {
      create_matcher_module_test()
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
