#include "regex/flagged.h"
#include "regex_types.h"
#include "ttest/ttest.h"


using namespace lex;

struct A {
  A(int k) : i(k) {}
  int i = 1;
};

using Flag = regex_constants::syntax_option_type;

using B = flagged<A, Flag>;

struct C {
  C(B b, double dub) : c {b.i}, d {dub} {}

  int c;
  double d;
};

using D = flagged<C, Flag>;

void flagged_test(ttest::error_log& log) {
  B b(Flag::icase, 10);

  if (b.flag() != Flag::icase) {
    log.append("flag method");
  }

  C test(b, 5.4);

  if (test.c != 10) {
    log.append("C");
  }

  D D_test(b, 7.8);

  if (D_test.c != 10) {
    log.append("D");
  }
}

ttest::test_suite::pointer create_flagged_test() {
  using namespace ttest;
  return create_test("flagged", flagged_test);
}
