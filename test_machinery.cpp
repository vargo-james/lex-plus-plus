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

void test_suite::append_error(const std::string& msg) {
  auto message = name();
  if (!msg.empty()) {
    message.append("::" + msg);
  }
  errors_.push_back(message);
}

void test_suite::collect_errors(const std::vector<pointer>& subtests) {
  for (auto& test: subtests) {
    test->run_test();
    auto messages = test->error_list();
    qualify_errors(messages);
  }
}

void simple_test::do_test() {
  if (test_()) {
    append_error();
  }
}

compound_test::compound_test(const std::string& name, 
    std::initializer_list<pointer> component_tests)
  : test_suite(name) {
  for (auto& test_ptr: component_tests) {
    components.emplace_back(std::move(test_ptr));
  }
}

void compound_test::do_test() {
  collect_errors(components);
}

void test_suite::qualify_errors(const error_log& log) {
  for (const auto& msg: log) {
    append_error(msg);
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
