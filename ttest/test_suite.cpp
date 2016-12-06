/*
 * The test_suite class is implemented here.
 */

#include "ttest/test_suite.h"

namespace ttest {

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

void test_suite::qualify_errors(const error_log& log) {
  for (const auto& msg: log) {
    append_error(msg);
  }
}

}//namespace ttest

