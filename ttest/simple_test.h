#ifndef _simple_test_h_
#define _simple_test_h_

#include "ttest/test_suite.h"

#include <functional>
#include <memory>
#include <utility>

namespace ttest {

class simple_test : public test_suite {
 public:
  using test_suite::message_type;
  using test_type = std::function<void(error_log&)>;
  using alt_test_type = std::function<int()>;

  simple_test(message_type name, const test_type& test) 
    : test_suite(std::move(name)), 
      test_ {test} {}
  simple_test(message_type name, const alt_test_type& test)
    : simple_test(std::move(name), 
        [test](error_log& log) {if(test()) log.append();}) {}

 private:
  test_type test_;

  void do_test() override {test_(errors());}
};

template <typename F>
test_suite::pointer create_test(simple_test::message_type name, F test) {
  return make_pointer<simple_test>{}(std::move(name), test);
  //return std::make_unique<simple_test>(std::move(name), test);
}

}//namespace ttest
#endif// _simple_test_h_
