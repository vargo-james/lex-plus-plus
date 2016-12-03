/* 
 * In this file, the test functions of the matcher_transition_class
 * are defined.
 */ 

#include "matcher/matcher_transition.h"
#include "test_machinery.h"

#include <ostream>

using namespace lex;

int matcher_trans_test(std::ostream& os) {
  matcher_transition<char> trans;

  for (auto ch = 'a'; ch != 'z'; ++ch) {
    if (trans.update(ch) != match_state::MISMATCH) {
      return 1;
    }
  }
  return 0;
}

