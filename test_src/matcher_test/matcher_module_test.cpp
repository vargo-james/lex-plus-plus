#include "matcher_test.h"
#include "ttest/ttest.h"

ttest::test_suite::pointer create_matcher_test();
ttest::test_suite::pointer create_matcher_impl_test();
ttest::test_suite::pointer create_atomic_test();
ttest::test_suite::pointer create_string_literal_test();
ttest::test_suite::pointer create_replication_test();
ttest::test_suite::pointer create_concatenation_test();
ttest::test_suite::pointer create_alternation_test();

//ttest::test_suite::pointer create_matcher_alt_test();

ttest::test_suite::pointer create_matcher_module_test() {
  using ttest::create_test;

  return create_test("matcher", {
      create_matcher_test(),
      create_matcher_impl_test(),
      create_atomic_test(),
      create_string_literal_test(),
      create_replication_test(),
      create_concatenation_test(),
      create_alternation_test(),
      //create_matcher_alt_test(),
  });
}
