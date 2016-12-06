/*
 * The test_suite class is a mechanism for composing a group of tests into
 * a conglomerate which is itself a test. It has two derived classes:
 * simple_test, which is constructed from a single test function; and
 * compound_test, which is constructed from an initializer list of 
 * pointers to test_suite objects.
 */

#ifndef _test_suite_h_
#define _test_suite_h_

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace ttest {

class test_suite {
 public:
  using error_log = std::vector<std::string>;
  using pointer = std::shared_ptr<test_suite>;

  test_suite(const std::string& name)
    : unit_name_ {name} {}

  void run_test() {do_test();}

  void report_errors(std::ostream& os) const;
  size_t error_count() const {return errors_.size();}

 protected:
  void append_error(const std::string& msg);
  void append_error() {append_error({});}
  void collect_errors(const std::vector<pointer>& subtests);

 private:
  virtual void do_test() = 0; 
  void qualify_errors(const error_log& log);
  std::string name() const {return unit_name_;}
  error_log& error_list() {return errors_;}

  std::string unit_name_;
  error_log errors_;

};


}//namespace ttest
#endif// _test_suite_h_
