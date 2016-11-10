/* This file tests the match_state_transition class.*/

#include "matcher_state_transition.h"


int test_update() {
  using namespace lex;

  match_state_transition<char> trans;

  for (auto ch = 'a'; ch != 'z'; ++ch) {
    if (trans.update(ch) != match_state::MISMATCH) {
      return 1;
    }
  }
  return 0;
}


