#include "matcher_test.h"
#include "ttest/ttest.h"

ttest::test_suite::pointer create_matcher_test();
ttest::test_suite::pointer create_matcher_transition_test();
ttest::test_suite::pointer create_atomic_test();
ttest::test_suite::pointer create_string_literal_test();
ttest::test_suite::pointer create_replication_test();
ttest::test_suite::pointer create_concatenation_test();
ttest::test_suite::pointer create_alternation_test();


ttest::test_suite::pointer create_matcher_module_test() {
  using namespace ttest;

  return create_test("matcher", {
      create_matcher_test(),
      create_matcher_transition_test(),
      create_atomic_test(),
      create_string_literal_test(),
      create_replication_test(),
      create_concatenation_test(),
      create_alternation_test(),
  });
}