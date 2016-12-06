/* 
 * The simple_test class is implemented in this file.
 */

#include "ttest/simple_test.h"

namespace ttest {
simple_test::simple_test(const message_type& name, const alt_test_type& test)
  : simple_test(name, [test](error_log& log) { 
      if(test()) log.append();
    }) {}

} //namespace ttest
