#include "simple_buffer_test.h"
#include "regex/simple_buffer.h"

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

  simple_buffer<char> buff2;
  buff2.push('a');
  if (buff2.empty()) {
    log.append("constructor");
  }
}

void buffer_test_sizen(ttest::error_log& log) {
  simple_buffer<char,3> buff;
  bool push3times = buff.push('a') && buff.push('b') && buff.push('c');
  if (!push3times) {
    log.append("push 3 times");
  }
  if (buff.push('d')) {
    log.append("pushing a full buffer");
  }
  char ch;
  buff.get(ch);
  buff.get(ch);
  if (ch != 'b') {
    log.append("b get");
  }
  buff.get(ch);
  if (ch != 'a') {
    log.append("not a stack");
  }

  if (buff.get(ch)) {
    log.append("empty get");
  }

}
