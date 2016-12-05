/*
 * In this file, we run all the tests.
 */
#include "matcher_test/matcher_module_test.h"
#include "test_machinery.h"

#include <iostream>


int main() {
  using std::cerr;

  auto lib_test_ptr = create_test("library test", {
      create_matcher_module_test()
    });
  lib_test_ptr->run_test();
  lib_test_ptr->report_errors(std::cerr);

  auto error_count = lib_test_ptr->error_count();

  if (error_count > 0) {
    std::cerr << '\n' << error_count << " errors detected.\n";
  }
  else {
    std::cerr << "No errors detected.\n";
  }

  return 0;
}
