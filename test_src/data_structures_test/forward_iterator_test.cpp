#include "data_structures/forward_iterator.h"
#include "ttest/ttest.h"

#include <algorithm>
#include <vector>
#include <list>

#include <iostream>

using namespace lex;
using std::list;
using std::vector;

void forward_iterator_test(ttest::error_log& log) {
  vector<int> v {1, 2, 3}; 
  list<int> l {10, 9, 8};

  auto begin = make_forward_iterator(v.begin());
  forward_iterator<int> end {v.end()};

  forward_iterator<int, const int&> b {v.cbegin()};
  auto vb = v.begin();
  typename vector<int>::const_iterator cvb = vb;

  log.append_if("vector error", !std::equal(begin, end, v.begin(), v.end()));

  begin = l.begin();
  end = l.end();
  log.append_if("list error", !std::equal(begin, end, l.begin(), l.end()));
}

ttest::test_suite::pointer create_forward_iterator_test() {
  using ttest::create_test;
  return create_test("forward_iterator", forward_iterator_test);
}
