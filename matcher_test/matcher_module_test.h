#ifndef _matcher_module_test_h_
#define _matcher_module_test_h_

#include "matcher_transition_test.h"
#include "matcher_test.h"
#include "atomic_test.h"
#include "alternation_test.h"
#include "concatenation_test.h"
#include "replication_test.h"
#include "ttest/ttest.h"

inline ttest::test_suite::pointer create_matcher_module_test() {
  using namespace ttest;

  return create_test("matcher", {
      create_matcher_test()
      , create_matcher_transition_test()
      , create_atomic_test(),
      create_replication_test(),
      create_concatenation_test(),
      create_alternation_test()
  });
}

#endif// _matcher_module_test_h_
