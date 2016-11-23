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
#include <iterator>
#include <numeric>
#include <ostream>
#include <string>
#include <vector>

class test_suite {
 public:
  using test_type = std::function<int(std::ostream&)>;

  test_suite() {}
  test_suite(const std::initializer_list<test_type>& list) 
    : tests_{list} {}

  void add_test(const test_type& test) {tests_.push_back(test);}

  int operator()(std::ostream& os) {
    using std::accumulate;
    using std::begin;
    using std::end;

    return accumulate(begin(tests_), end(tests_), 0, 
        [&os](int partial_sum, const auto& test) {
          return partial_sum + test(os);
        });
  }

 private:
  std::vector<test_type> tests_;
};

inline void test_log(std::ostream& os, const std::string& msg) {
  os << msg << '\n';
}

#endif// _test_machinery_h_
