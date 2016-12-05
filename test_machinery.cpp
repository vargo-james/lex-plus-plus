/*
 * This implements the aggregation of tests in the test_suite class.
 */

#include "test_machinery.h"

#include <utility>

void test_suite::report_errors(std::ostream& os) const {
  for (const auto& msg : errors_) {
    os << msg << '\n';
  }
}

void simple_test::run_test() {
  if (test_()) {
    error_list().push_back(name());
  }
}

compound_test::compound_test(const std::string& name, 
    std::initializer_list<pointer> component_tests)
  : test_suite(name) {
  for (auto& test_ptr: component_tests) {
    components.emplace_back(std::move(test_ptr));
  }
}

void compound_test::run_test() {
  for (auto& test: components) {
    test->run_test();
    auto messages = test->error_list();
    for (auto& msg: messages) {
      error_list().push_back(name() + "::" + msg);
    }
  }
}

test_suite::pointer create_test(const std::string& name, 
    const simple_test::test_type& test) {
  return std::make_shared<simple_test>(name, test);
}

test_suite::pointer create_test(const std::string& name, 
    const std::initializer_list<test_suite::pointer>& test_list) {
  return std::make_shared<compound_test>(name, test_list);
}
