/*
 * The test_suite class is implemented here.
 */

#include "ttest/test_suite.h"

namespace ttest {

void test_suite::collect_errors(const std::vector<pointer>& subtests) {
  for (auto& test: subtests) {
    test->run_test();
    auto messages = test->errors_;
    errors_.incorporate(messages);
  }
}

}//namespace ttest

