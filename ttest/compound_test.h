#ifndef _compound_test_h_
#define _compound_test_h_

#include "ttest/test_suite.h"

#include <initializer_list>
#include <vector>

namespace ttest {

class compound_test : public test_suite {
 public:
  using test_suite::message_type;
  using test_suite::pointer;
  using initializer_list = std::initializer_list<pointer>;

  compound_test(const message_type& name, const initializer_list& tests) 
    : test_suite(name), 
      components(tests) {}

 private:
  std::vector<pointer> components;

  void do_test() override;
};

test_suite::pointer create_test(const compound_test::message_type& name, 
    const compound_test::initializer_list& test_list);

}//namespace ttest
#endif// _compound_test_h_
