#ifndef _compound_test_h_
#define _compound_test_h_

#include "ttest/test_suite.h"

#include <initializer_list>
#include <memory>
#include <vector>

namespace ttest {

class compound_test : public test_suite {
 public:
  using test_suite::message_type;
  using test_suite::pointer;

  /*
  compound_test(message_type name, std::initializer_list<pointer> tests) 
    : test_suite(std::move(name)), 
      components(tests) {}
      */
  compound_test(message_type name, std::initializer_list<pointer> tests) 
    : test_suite(std::move(name)) {
      components.reserve(tests.size());
      for (auto& ptr : tests) {
        components.push_back(std::move(ptr));
      }
    }

 private:
  std::vector<pointer> components;

  void do_test() override {this->collect_errors(components);}
};

inline test_suite::pointer create_test(compound_test::message_type name, 
    std::initializer_list<test_suite::pointer> test_list) {
  return make_pointer<compound_test>{}(std::move(name), test_list);
  //return std::make_unique<compound_test>(std::move(name), test_list);
}

}//namespace ttest
#endif// _compound_test_h_
