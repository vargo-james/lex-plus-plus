#ifndef _simple_test_h_
#define _simple_test_h_

#include "ttest/test_suite.h"

#include <functional>
#include <string>

namespace ttest {

class simple_test : public test_suite {
 public:
  using test_type = std::function<int()>;

  simple_test(const std::string& name, const test_type& test) 
    : test_suite(name), 
      test_ {test} {}

 private:
  test_type test_;

  void do_test() override;
};

test_suite::pointer create_test(const std::string& name, 
    const simple_test::test_type& test);

}//namespace ttest
#endif// _simple_test_h_
