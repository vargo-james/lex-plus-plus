#ifndef _simple_buffer_test_h_
#define _simple_buffer_test_h_

#include "ttest/ttest.h"


void buffer_test_size1(ttest::error_log& log);
void buffer_test_sizen(ttest::error_log& log);

inline ttest::test_suite::pointer create_buffer_test() {
  using namespace ttest;
  return create_test("simple_buffer", {
      create_test("size1", buffer_test_size1),
      create_test("sizen", buffer_test_sizen)
  });
}

#endif// _simple_buffer_test_h_
