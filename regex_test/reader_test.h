#ifndef _reader_test_h_
#define _reader_test_h_

#include "ttest/ttest.h"

void reader_literals_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_reader_test() {
  using namespace ttest;
  return create_test("reader", {
      create_test("literals", reader_literals_test)
      });
}
#endif// _reader_test_h_
