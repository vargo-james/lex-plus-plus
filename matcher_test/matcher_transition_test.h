/* This file tests the matcher_transition class.*/

#include "matcher/matcher_transition.h"


int matcher_transition_test() {
  using namespace lex;

  matcher_transition<char> trans;

  for (auto ch = 'a'; ch != 'z'; ++ch) {
    if (trans.update(ch) != match_state::MISMATCH) {
      return 1;
    }
  }
  return 0;
}


