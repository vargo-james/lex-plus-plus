#ifndef _matcher_module_test_h_
#define _matcher_module_test_h_

#include "matcher_transition_test.h"
#include "matcher_test.h"
#include "atomic_matcher_test.h"
#include "test_machinery.h"

inline int matcher_module_test(std::ostream& os) {
  test_suite module_test {
    matcher_test, 
    matcher_transition_test,
    atomic_matcher_test
  };
  return module_test(os);
}

#endif// _matcher_module_test_h_
