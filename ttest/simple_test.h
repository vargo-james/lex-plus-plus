#ifndef _simple_test_h_
#define _simple_test_h_

#include "ttest/test_suite.h"

#include <functional>
#include <memory>

namespace ttest {

class simple_test : public test_suite {
 public:
  using test_suite::message_type;
  using test_type = std::function<void(error_log&)>;
  using alt_test_type = std::function<int()>;

  simple_test(const message_type& name, const test_type& test) 
    : test_suite(name), 
      test_ {test} {}
  simple_test(const message_type& name, const alt_test_type& test);

 private:
  test_type test_;

  void do_test() override {test_(errors());}
};

template <typename F>
test_suite::pointer create_test(const simple_test::message_type& name, 
    const F& test) {
  return std::make_shared<simple_test>(name, test);
}

}//namespace ttest
#endif// _simple_test_h_
