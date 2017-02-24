#include "data_structures/simple_buffer.h"
#include "ttest/ttest.h"

#include <iostream>

using namespace lex;

void buffer_test_size1(ttest::error_log& log) {
  simple_buffer<int> buff;
  if (!buff.empty()) {
    log.append("default full");
  }

  auto push = buff.push(5);
  if (!push) {
    log.append("pushing empty");
  }

  push = buff.push(3);
  if (push) {
    log.append("pushing full");
  }

  auto get = buff.get();
  if (!get) {
    log.append("getting full");
  }
  auto out = *get;

  get = buff.get();
  if (get) {
    log.append("getting empty");
  }

  if (out != 5) {
    log.append("value");
  }

  simple_buffer<char> buff2;
  buff2.push('a');
  if (buff2.empty()) {
    log.append("constructor");
  }
}

void buffer_test_sizen(ttest::error_log& log) {
  simple_buffer<char,unsigned char,3> buff;
  bool push3times = buff.push('a') && buff.push('b') && buff.push('c');
  if (!push3times) {
    log.append("push 3 times");
  }
  if (buff.push('d')) {
    log.append("pushing a full buffer");
  }
  buff.get();
  auto get = buff.get();
  auto ch = *get;
  if (ch != 'b') {
    log.append("b get");
  }
  ch = *(buff.get());
  if (ch != 'a') {
    log.append("not a stack");
  }

  if (buff.get()) {
    log.append("empty get");
  }

}

ttest::test_suite::pointer create_simple_buffer_test() {
  using namespace ttest;
  return create_test("simple_buffer", {
      create_test("size1", buffer_test_size1),
      create_test("sizen", buffer_test_sizen)
  });
}
