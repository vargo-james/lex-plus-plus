#ifndef _compound_test_h_
#define _compound_test_h_

#include "ttest/test_suite.h"

#include <initializer_list>

namespace ttest {

class compound_test : public test_suite {
 public:
  using test_suite::error_log;
  using test_suite::pointer;

  compound_test(const std::string& name, 
      std::initializer_list<pointer> component_tests);

 private:
  std::vector<pointer> components;

  void do_test() override;
};

test_suite::pointer create_test(const std::string& name, 
    const std::initializer_list<test_suite::pointer>& test_list);

}//namespace ttest
#endif// _compound_test_h_
