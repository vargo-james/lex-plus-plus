#ifndef _matcher_module_test_h_
#define _matcher_module_test_h_

#include "matcher_transition_test.h"
#include "matcher_test.h"
#include "atomic_matcher_test.h"
#include "test_machinery.h"

inline test_suite::pointer create_matcher_module_test() {
  return create_test("matcher module", {
      create_matcher_test()
      , create_matcher_transition_test()
      , create_atomic_matcher_test()
  });
}

#endif// _matcher_module_test_h_
