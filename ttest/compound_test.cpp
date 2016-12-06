#include "ttest/compound_test.h"

#include <memory>

namespace ttest {

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

test_suite::pointer create_test(const std::string& name, 
    const std::initializer_list<test_suite::pointer>& test_list) {
  return std::make_shared<compound_test>(name, test_list);
}

}//namespace ttest

