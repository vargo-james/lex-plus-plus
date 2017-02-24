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
#include <utility>
#include <vector>

namespace ttest {
/*
 * The following struct is used to control the use of shared_ptr in one place.
 * Ideally, we would use unique_ptr since there is no shared ownership
 * in this library. Instead shared_ptrs are used as a hack with the sole 
 * purpose of allowing the user the following notation to create a compound
 * test:
 *
 *  auto pointer_to_my_test = create_test("test_name", {
 *    create_test("subtest1", test1),
 *    create_test("subtest2", test2),
 *    create_submoduleA_test(),
 *    create_submoduleB_test(),
 *  });
 *
 *  A compound test is made up of a vector of pointers to subtests. This 
 *  notation initializes that vector with an initializer_list of pointers.
 *  However, initializer_lists are always const and can only be iterated
 *  over with const iterators. They can only be copied from, never moved from. 
 *  This is a problem for unique_ptr. So I use shared_ptr instead as a hack.
 *  It avoids deep copying at the cost of maintaining reference counting.
 */

template <typename T>
struct make_pointer{
  using pointer = std::shared_ptr<T>;
  template <typename... Args>
  pointer operator()(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }
};

class test_suite {
 public:
  using message_type = typename error_log::message_type;
  using ostream = typename error_log::ostream;
  using pointer = typename make_pointer<test_suite>::pointer;

  test_suite(message_type name)
    : errors_(std::move(name)) {}

  void run_test() {do_test();}

  void report(ostream& os) const {errors_.report(os);}
  std::size_t error_count() const {return errors_.size();}

  virtual ~test_suite() = default;
 protected:
  // This function should properly be a private method of the compound_test
  // class. However, it needs to access a protected method on separate
  // instances of test_suite objects. That is not directly possible.
  void collect_errors(const std::vector<pointer>& subtests);

  error_log& errors() {return errors_;}
 private:
  virtual void do_test() = 0; 

  error_log errors_;
};

inline void test_suite::collect_errors(const std::vector<pointer>& subtests) {
  for (auto& test: subtests) {
    test->run_test();
    errors_.incorporate(test->errors_);
  }
}

}//namespace ttest
#endif// _test_suite_h_
