#ifndef _match_state_transition_h_
#define _match_state_transition_h_

#include "regex_types.h"

#include <memory>

namespace lex {

template <typename Char>
class match_state_transition {
 public:
  using char_type = Char;
  using pointer = std::unique_ptr<match_state_transition>;

  match_state_transition() = default;
  ~match_state_transition() = default;

  virtual match_state 
  update(const char_type& ch) {return match_state::MISMATCH;}
  virtual match_state 
  initialize() {return match_state::MATCH;}
  virtual pointer
  clone() const {return std::make_unique<match_state_transition>();}
};

template <typename Derived, typename Char>
class matcher_transition_cloner : public match_state_transition<Char> {
 public:
  using typename match_state_transition<Char>::pointer;

  pointer clone() const override {
    return std::make_unique<Derived>(static_cast<Derived const&>(*this));
  }
};

template <typename Char>
using state_transition_ptr = 
  std::unique_ptr<match_state_transition<Char>>;

}//namespace lex
#endif// _match_state_transition_h_
