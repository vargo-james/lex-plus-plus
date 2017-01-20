#ifndef token_stream_test_h_
#define token_stream_test_h_

#include "ttest/ttest.h"

namespace lex {

void literals_test(ttest::error_log& log);

inline ttest::test_suite::pointer create_token_stream_test() {
  using namespace ttest;
  return create_test("token_stream", {
      create_test("literals", literals_test)
  });

}

}//namespace lex
#endif// token_stream_test_h_
