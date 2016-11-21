#ifndef _matcher_module_test_h_
#define _matcher_module_test_h_

#include "matcher_transition_test.h"
#include "matcher_test.h"

int matcher_module_test() {
  int error_count {0};

  error_count += matcher_transition_test();
  error_count += matcher_test();

  return error_count;
}

#endif// _matcher_module_test_h_
