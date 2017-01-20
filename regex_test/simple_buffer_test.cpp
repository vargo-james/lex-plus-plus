#include "simple_buffer_test.h"
#include "regex/simple_buffer.h"

using namespace lex;

void simple_buffer_test(ttest::error_log& log) {
  simple_buffer<int> buff;
  if (!buff.empty()) {
    log.append("default full");
  }

  auto set = buff.set(5);
  if (!set) {
    log.append("setting empty");
  }

  set = buff.set(3);
  if (set) {
    log.append("setting full");
  }

  int out;
  auto get = buff.get(out);
  if (!get) {
    log.append("getting full");
  }

  get = buff.get(out);
  if (get) {
    log.append("getting empty");
  }

  if (out != 5) {
    log.append("value");
  }

  simple_buffer<char> buff2('a');
  if (buff2.empty()) {
    log.append("constructor");
  }

  auto ch = buff2.peek();

  if (ch != 'a') {
    log.append("peek");
  }
}
