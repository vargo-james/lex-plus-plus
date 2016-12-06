/* 
 * The simple_test class is implemented in this file.
 */

#include "ttest/simple_test.h"

namespace ttest {

void simple_test::do_test() {
  if (test_()) {
    append_error();
  }
}

test_suite::pointer create_test(const std::string& name, 
    const simple_test::test_type& test) {
  return std::make_shared<simple_test>(name, test);
}

} //namespace ttest
