/*
 * The test_suite class is a mechanism for composing a group of tests into
 * a conglomerate which is itself a test. It has two derived classes:
 * simple_test, which is constructed from a single test function; and
 * compound_test, which is constructed from an initializer list of 
 * pointers to test_suite objects.
 */

#ifndef _test_suite_h_
#define _test_suite_h_

#include "ttest/error_log.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace ttest {

class test_suite {
 public:
  using message_type = error_log::message_type;
  using ostream = error_log::ostream;
  using pointer = std::shared_ptr<test_suite>;

  test_suite(const message_type& name)
    : errors_(name) {}

  void run_test() {do_test();}

  void report(ostream& os) const {errors_.report(os);}
  size_t error_count() const {return errors_.size();}

  virtual ~test_suite() = default;
 protected:
  void append_error(const message_type& msg) {errors_.append(msg);}
  void append_error() {errors_.append();}
  // This function should properly be a private method of the compound_test
  // class. However, it needs to access a protected method on separate
  // instances of test_suite objects. That is not directly possible.
  void collect_errors(const std::vector<pointer>& subtests);

  error_log& errors() {return errors_;}

 private:
  virtual void do_test() = 0; 

  error_log errors_;
};


}//namespace ttest
#endif// _test_suite_h_
