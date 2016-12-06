/*
 * In this project, a test is a callable object with signature
 * int(std::ostream&). The ostream reference is the mechanism by which 
 * tests can report any details back to the user. 
 * The return value is a mechanism for counting the number of errors 
 * encountered. Thus a return value of 0 is given if no errors are found.
 *
 * The test_suite class is a mechanism for composing a group of tests into
 * a conglomerate which is itself a test. Thus simple unit tests can be
 * defined using a function, a lambda or anything else that can be 
 * bound to a std::function<int(std::ostream&)> object. Such tests can be 
 * combined using the test_suite class. Then these can in turn be combined
 * into tests of still larger logical units.
 */

#ifndef _test_machinery_h_
#define _test_machinery_h_

#include <cstddef>
#include <functional>
#include <initializer_list>
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
#endif// _test_machinery_h_
