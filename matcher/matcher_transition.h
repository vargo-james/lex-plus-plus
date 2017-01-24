#ifndef _matcher_transition_h_
#define _matcher_transition_h_

#include "regex_types.h"

#include <memory>

namespace lex {
namespace detail {

// A matcher_transition object manages the state transitions for the 
// matcher class. It does the work for both the update() and initialize()
// methods.
template <typename CharT>
class matcher_transition {
 public:
  using value_type = CharT;
  using pointer = std::unique_ptr<matcher_transition>;

  matcher_transition() = default;
  ~matcher_transition() = default;

  virtual match_state 
  update(const value_type& ch) {return match_state::MISMATCH;}
  virtual match_state 
  initialize() {return match_state::FINAL_MATCH;}
  virtual pointer
  clone() const {return std::make_unique<matcher_transition>();}
};

// This bit of CRTP automatically correctly defines the clone
// method for all classes derived from this cloner class.
template <typename Derived, typename CharT>
class matcher_transition_cloner : public matcher_transition<CharT> {
 public:
  using typename matcher_transition<CharT>::pointer;

  pointer clone() const override {
    return std::make_unique<Derived>(static_cast<Derived const&>(*this));
  }
};

}//namespace detail
}//namespace lex
#endif// _matcher_transition_h_
