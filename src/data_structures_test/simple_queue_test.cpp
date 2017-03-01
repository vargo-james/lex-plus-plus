#include "ttest/ttest.h"
#include "data_structures/simple_queue.h"

#include <string>

using namespace lex;
using std::to_string;

void simple_queue_test(ttest::error_log& log) {

  constexpr unsigned Capacity {5};
  simple_queue<unsigned, unsigned, Capacity> queue;

  log.append_if("empty and full", queue.full());

  log.append_if("incorrect capacity: " + to_string(queue.capacity()) , 
      queue.capacity() != Capacity);

  for (auto i = 0u; i < queue.capacity(); ++i) {
    log.append_if("prematurely full: " + to_string(i), queue.full());
    log.append_if("unable to enqueue", !queue.enqueue(i));
  }

  log.append_if("not full", !queue.full());

  log.append_if("enqueueing while full", queue.enqueue(6));

  auto i = 0u;
  for (; !queue.empty(); ++i) {
    auto ip = queue.get();
    log.append_if("unable to get", !ip);

    log.append_if("invalid entry", *ip != i);
  }

  log.append_if("incorrect number of get()s: " + to_string(i), 
      i != queue.capacity());
}

ttest::test_suite::pointer create_simple_queue_test() {
  using ttest::create_test;
  return create_test("simple_queue", simple_queue_test);
}
