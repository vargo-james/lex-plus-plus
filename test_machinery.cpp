/*
 * This implements the aggregation of tests in the test_suite class.
 */

#include "test_machinery.h"

#include <ostream>
#include <utility>

void test_suite::report_errors(std::ostream& os) const {
  for (const auto& msg : errors_) {
    os << msg << '\n';
  }
}

int test_suite::operator()(std::ostream& os) const {
  int error_count {0};

  for (const auto& test: tests_) {
    auto error = test(os);
    if (error) {
      error_count += error;
      report_error(os);
    }
  }
  return error_count;
}

void simple_test::run_test(std::ostream& os) {
  if (test_(os)) {
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

void compound_test::run_test(std::ostream& os) {
  for (auto& test: components) {
    test->run_test(os);
    auto messages = test->error_list();
    for (auto& msg: messages) {
      error_list().push_back(name() + "::" + msg);
    }
  }
}

test_suite::pointer create_test(const std::string& name, 
    const test_suite::test_type& test) {
  return std::make_shared<simple_test>(name, test);
}

test_suite::pointer create_test(const std::string& name, 
    const std::initializer_list<test_suite::pointer>& test_list) {
  return std::make_shared<compound_test>(name, test_list);
}
