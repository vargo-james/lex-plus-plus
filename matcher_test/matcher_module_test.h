#ifndef _matcher_module_test_h_
#define _matcher_module_test_h_

#include "matcher_transition_test.h"
#include "matcher_test.h"
#include "atomic_matcher_test.h"
#include "test_machinery.h"

const test_suite matcher_module_test(std::string("matcher module"), {
    matcher_test, 
    matcher_transition_test,
    atomic_matcher_test,
  });

#endif// _matcher_module_test_h_
