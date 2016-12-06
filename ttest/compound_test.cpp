#include "ttest/compound_test.h"

#include <memory>
#include <utility>

namespace ttest {

void compound_test::do_test() {
  collect_errors(components);
}

test_suite::pointer create_test(const compound_test::message_type& name, 
    const compound_test::initializer_list& test_list) {
  return std::make_shared<compound_test>(name, test_list);
}

}//namespace ttest

