/*
 * This implements the aggregation of tests in the test_suite class.
 */

#include "test_machinery.h"

#include <ostream>


int test_suite::operator()(std::ostream& os) const {
  int error_count {0};

  for (const auto& test: tests_) {
    auto error = test(os);
    if (error) {
      error_count += error;
      report_error(os);
      //os << unit_name_ << ": ";
    }
  }
  return error_count;
}


