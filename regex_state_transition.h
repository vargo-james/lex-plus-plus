#ifndef _regex_state_transition_h_
#define _regex_state_transition_h_

#include "regex_types.h"

#include <memory>

namespace token_iterator {

template <typename Char>
class regex_state_transition {
 public:
  using char_type = Char;
  using pointer = std::unique_ptr<regex_state_transition>;

  regex_state_transition() = default;
  ~regex_state_transition() = default;

  virtual regex_state 
  update(const char_type& ch) {return regex_state::MISMATCH;}
  virtual regex_state 
  initialize() {return regex_state::MATCH;}
  virtual pointer
  clone() const {return std::make_unique<regex_state_transition>();}
};

template <typename Derived, typename Char>
class regex_transition_cloner : public regex_state_transition<Char> {
 public:
  using typename regex_state_transition<Char>::pointer;

  pointer clone() const override {
    return std::make_unique<Derived>(static_cast<Derived const&>(*this));
  }
};

template <typename Char>
using state_transition_ptr = 
  std::unique_ptr<regex_state_transition<Char>>;

}//namespace token_iterator
#endif// _regex_state_transition_h_
