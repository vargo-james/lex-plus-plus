#include "ttest/ttest.h"

ttest::test_suite::pointer create_simple_buffer_test();
ttest::test_suite::pointer create_simple_queue_test();
ttest::test_suite::pointer create_forward_iterator_test();

ttest::test_suite::pointer create_data_structures_module_test() {
  using ttest::create_test;
  return create_test("data_structures", {
      create_simple_buffer_test(),
      create_simple_queue_test(),
      create_forward_iterator_test(),
    });
}
