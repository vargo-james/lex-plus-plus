#ifndef _atomic_h_
#define _atomic_h_

#include "matcher/matcher.h"

#include <vector>

namespace lex {

// The Regex transition object corresponding to a single char.
namespace detail {
template <typename CharT>
struct singleton_matcher_transition 
  : public matcher_transition_cloner<
           singleton_matcher_transition<CharT>, CharT
         > {
 public:
  using typename matcher_transition<CharT>::char_type;

  singleton_matcher_transition(const char_type& t)
    : match {t} {}

  // This only matches when it was previously UNDECIDED and the 
  // character it receives (t), is a match.
  match_state update(const char_type& t) override;
  match_state initialize() override;
 private:
  char_type match;
};

template <typename CharT>
match_state singleton_matcher_transition<CharT>::update(const char_type& t) {
  if (t == match) {
    return match_state::FINAL_MATCH;
  }
  return match_state::MISMATCH;
}

template <typename CharT>
match_state singleton_matcher_transition<CharT>::initialize() {
  return match_state::UNDECIDED;
}

}//namespace detail
template <typename CharT>
matcher<CharT> singleton_matcher(const CharT& ch) {
  matcher_factory<detail::singleton_matcher_transition<CharT>> fac;
  return fac.create(ch);
}


// The matcher object corresponding to a predicate on a single char. 
// E.g. [:alpha:]
// In order for the initialize() method to be semantically correct, 
// the predicate Pred must be stateless.
namespace detail {
template <typename CharT>
class predicate_matcher_transition 
  : public matcher_transition_cloner<
             predicate_matcher_transition<CharT>, CharT
           > {
 public:
  using typename matcher_transition<CharT>::char_type;
  using predicate_type = predicate_type_t<CharT>;

  predicate_matcher_transition(const predicate_type& p) 
    : pred {p} {}

  match_state update(const CharT& t) override;
  match_state initialize() override;
 private:
  predicate_type pred;
};

template <typename CharT>
match_state 
predicate_matcher_transition<CharT>::update(const CharT& t) {
  if (pred(t)) {
    return match_state::FINAL_MATCH;
  }
  return match_state::MISMATCH;
}

template <typename CharT>
match_state predicate_matcher_transition<CharT>::initialize() {
  return match_state::UNDECIDED;
}
}//namespace detail
template <typename CharT>
matcher<CharT> predicate_matcher(const predicate_type_t<CharT>& p) {
  matcher_factory<detail::predicate_matcher_transition<CharT>> fac;
  return fac.create(p);
}

/*
template <typename CharT>
struct always_true {
  bool operator()(const CharT&) {return true;}
};*/

// This creates a matcher that matches a '.'. The predicate
// always returns true.
template <typename CharT>
matcher<CharT> universal_singleton_matcher() {
  return predicate_matcher<CharT>([](const auto&){return true;});
  //return predicate_matcher<CharT>(always_true<CharT>{});
}


namespace detail {
template <typename Iterator>
class string_matcher_transition 
  : public matcher_transition_cloner<string_matcher_transition<Iterator>, 
      char_type_t<Iterator>> {
 public:
  using char_type = char_type_t<Iterator>;
  using string_type = std::vector<char_type>;
  using size_type = typename string_type::size_type;

  string_matcher_transition(Iterator begin, Iterator end)
    : string(begin, end),
      current {0} {}

  match_state update(const char_type& t) override;
  match_state initialize() override;
 private:
  string_type string;
  size_type current;
};  

template <typename Iterator> 
match_state 
string_matcher_transition<Iterator>::update(const char_type& t) {
  if (t == string[current]) {
    ++current;
    if (current == string.size()) return match_state::FINAL_MATCH;
    return match_state::UNDECIDED;
  }
  return match_state::MISMATCH;
}

template <typename Iterator>
match_state string_matcher_transition<Iterator>::initialize() {
  current = 0;
  return string.empty()? match_state::FINAL_MATCH : match_state::UNDECIDED;
}
}//namespace detail

template <typename Iterator>
matcher<char_type_t<Iterator>>
string_matcher(Iterator begin, Iterator end) {
  matcher_factory<detail::string_matcher_transition<Iterator>> fac;
  return fac.create(begin, end);
}


template <typename String>
matcher<typename String::value_type>
string_matcher(const String& str) {
  return string_matcher(str.begin(), str.end());
}

}//namespace lex
#endif// _atomic_h_
