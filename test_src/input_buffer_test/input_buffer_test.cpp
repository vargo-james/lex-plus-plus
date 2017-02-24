#include "input_buffer_test.h"
#include "input_buffer/input_buffer.h"

#include <ios>
#include <iostream>
#include <iterator>

void input_buffer_test(ttest::error_log& log) {
  using namespace lex;
  using std::istream_iterator;
  using std::cin;
  using std::cout;
  using std::noskipws;

  cout << "Enter a line. If you do not see an immediate response, "
      "this test has failed.\n";

  // Initializing the istream_iterator initiates a read from cin.
  // Therefore, this should not be done until you are ready to read.
  using Iterator = istream_iterator<char>;
  noskipws(cin);
  input_buffer<Iterator> buffer(Iterator(cin), Iterator{});

  auto begin = buffer.begin();
  auto end = buffer.end();

  while (begin != end) {
    if (*begin++ == '\n') {
      cout << "Thank you\n";
    }
  }
}
