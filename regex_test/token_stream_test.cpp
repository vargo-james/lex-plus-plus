#include "token_stream_test.h"
#include "regex/token_stream.h"

#include <string>

using namespace lex;

void literals_test(ttest::error_log& log) {
  std::string reg{"ab(?=pq)[abb[:alpha:]xy]qt{1,5}"};
  return;
}
