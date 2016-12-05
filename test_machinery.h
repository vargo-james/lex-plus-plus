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

#include <functional>
#include <initializer_list>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

class test_suite {
 public:
  using pointer = std::shared_ptr<test_suite>;
  using test_type = std::function<int(std::ostream&)>;

  test_suite(const std::string& name)
    : unit_name_ {name} {}
  test_suite(const std::string& name,
      const std::initializer_list<test_type>& list)
    : unit_name_ {name},
      tests_{list} {}

  void add_test(const test_type& test) {tests_.push_back(test);}

  int operator()(std::ostream& os) const;

  virtual void run_test(std::ostream& os) {}
  void report_errors(std::ostream& os) const;
  size_t error_count() const {return errors_.size();}

  std::vector<std::string>& error_list() {return errors_;}
 protected:
  std::string name() const {return unit_name_;}

 private:
  std::string unit_name_;
  std::vector<test_type> tests_;
  std::vector<std::string> errors_;

  void report_error(std::ostream& os) const {
    os << name() << ": ";
  }
};

class simple_test : public test_suite {
 public:
  using test_suite::test_type;

  simple_test(const std::string& name, const test_type& test) 
    : test_suite(name), 
      test_ {test} {}

  void run_test(std::ostream& os) override;

 private:
  test_type test_;
};

class compound_test : public test_suite {
 public:
  using test_suite::pointer;

  compound_test(const std::string& name, 
      std::initializer_list<pointer> component_tests);

  void run_test(std::ostream& os) override;

 private:
  std::vector<pointer> components;
};

test_suite::pointer create_test(const std::string& name, 
    const test_suite::test_type& test);

test_suite::pointer create_test(const std::string& name, 
    const std::initializer_list<test_suite::pointer>& test_list);

#endif// _test_machinery_h_
