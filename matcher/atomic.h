#ifndef _atomic_h_
#define _atomic_h_

#include "matcher/matcher.h"

#include <locale>
#include <vector>

namespace lex {

// The Regex impl object corresponding to a single char.
namespace detail {
template <typename CharT, typename Traits>
struct singleton_matcher_impl 
  : public matcher_impl_cloner<
           singleton_matcher_impl<CharT, Traits>, CharT, Traits
         > {
 public:
  using typename matcher_impl<CharT, Traits>::value_type;
  using traits_type = Traits;

  singleton_matcher_impl(value_type t, const Traits& tr)
    : traits_ {tr},
      match(t) {}

  // This only matches when it was previously UNDECIDED and the 
  // character it receives (t), is a match.
  match_state update(value_type t) override;
  match_state initialize() override;
 private:
  const traits_type& traits_;
  value_type match;
};

template <typename CharT, typename Traits>
match_state singleton_matcher_impl<CharT, Traits>::update(value_type t) {
  if (t == match) {
    return match_state::FINAL_MATCH;
  }
  return match_state::MISMATCH;
}

template <typename CharT, typename Traits>
match_state singleton_matcher_impl<CharT, Traits>::initialize() {
  return match_state::UNDECIDED;
}

}//namespace detail
template <typename CharT, typename Traits>
matcher<CharT, Traits> singleton_matcher(CharT ch, const Traits& traits) {
  matcher_factory<detail::singleton_matcher_impl<CharT, Traits>> fac;
  return fac.create(ch, traits);
}


// The matcher object corresponding to a predicate on a single char. 
// E.g. [:alpha:]
// In order for the initialize() method to be semantically correct, 
// the predicate Pred must be stateless.
namespace detail {
template <typename CharT, typename Traits>
class predicate_matcher_impl 
  : public matcher_impl_cloner<
             predicate_matcher_impl<CharT, Traits>, CharT, Traits
           > {
 public:
  using typename matcher_impl<CharT, Traits>::value_type;
  using predicate_type = predicate_type_t<CharT>;

  predicate_matcher_impl(predicate_type p) 
    : pred {p} {}

  match_state update(CharT t) override;
  match_state initialize() override;
 private:
  predicate_type pred;
};

template <typename CharT, typename Traits>
match_state 
predicate_matcher_impl<CharT, Traits>::update(CharT t) {
  if (pred(t)) {
    return match_state::FINAL_MATCH;
  }
  return match_state::MISMATCH;
}

template <typename CharT, typename Traits>
match_state predicate_matcher_impl<CharT, Traits>::initialize() {
  return match_state::UNDECIDED;
}
}//namespace detail
template <typename CharT, typename Traits>
matcher<CharT, Traits> predicate_matcher(predicate_type_t<CharT> p) {
  matcher_factory<detail::predicate_matcher_impl<CharT, Traits>> fac;
  return fac.create(p);
}

// This creates a matcher that matches a '.'. The predicate
// always returns true.
template <typename CharT, typename Traits>
matcher<CharT, Traits> universal_singleton_matcher() {
  return predicate_matcher<CharT, Traits>([](auto){return true;});
}

template <typename CharT, typename Traits>
matcher<CharT, Traits> character_class_matcher(CharT ch, const Traits& traits) {
  auto loc = traits.getloc();
  switch (ch) {
  case CharT('.'):
    return predicate_matcher<CharT, Traits>([](auto ch) {return true;});
  case CharT('w'):
    return predicate_matcher<CharT, Traits>([loc](auto ch) {
          return std::isalnum(ch, loc) || ch == CharT('_');
        });
  case CharT('W'):
    return predicate_matcher<CharT, Traits>([loc](auto ch) {
          return !std::isalnum(ch, loc) && ch != CharT('_');
        });
  case CharT('d'):
    return predicate_matcher<CharT, Traits>([loc](auto ch) {
          return std::isdigit(ch, loc);
        });
  case CharT('D'):
    return predicate_matcher<CharT, Traits>([loc](auto ch) {
          return !std::isdigit(ch, loc);
        });
  case CharT('s'):
    return predicate_matcher<CharT, Traits>([loc](auto ch) {
          return std::isspace(ch, loc);
        });
  case CharT('S'):
    return predicate_matcher<CharT, Traits>([loc](auto ch) {
          return !std::isspace(ch, loc);
        });
  case CharT('l'):
    return predicate_matcher<CharT, Traits>([loc](auto ch) {
          return std::islower(ch, loc);
        });
  case CharT('L'):
    return predicate_matcher<CharT, Traits>([loc](auto ch) {
          return !std::islower(ch, loc);
        });
  case CharT('u'):
    return predicate_matcher<CharT, Traits>([loc](auto ch) {
          return std::isupper(ch, loc);
        });
  case CharT('U'):
    return predicate_matcher<CharT, Traits>([loc](auto ch) {
          return !std::isupper(ch, loc);
        });
  }
  return {};
}


}//namespace lex
#endif// _atomic_h_
