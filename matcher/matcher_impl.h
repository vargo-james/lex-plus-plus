#ifndef _matcher_impl_h_
#define _matcher_impl_h_

#include "regex_types.h"

#include <memory>

namespace lex {
// A matcher_impl object manages the state transitions for the 
// matcher class. It does the work for both the update() and initialize()
// methods.
template <typename CharT, typename Traits>
class matcher_impl {
 public:
  using value_type = CharT;
  using traits_type = Traits;
  using pointer = std::unique_ptr<matcher_impl>;

  matcher_impl() = default;
  ~matcher_impl() = default;

  virtual match_state 
  update(value_type ch) {return match_state::MISMATCH;}
  virtual match_state 
  initialize() {return match_state::FINAL_MATCH;}
  virtual pointer
  clone() const {return std::make_unique<matcher_impl>();}
};

// This bit of CRTP automatically correctly defines the clone
// method for all classes derived from this cloner class.
template <typename Derived, typename CharT, typename Traits>
class matcher_impl_cloner : public matcher_impl<CharT, Traits> {
 public:
  using typename matcher_impl<CharT, Traits>::pointer;

  pointer clone() const override {
    return std::make_unique<Derived>(static_cast<Derived const&>(*this));
  }
};

}//namespace lex
#endif// _matcher_impl_h_
