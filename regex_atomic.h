#ifndef _regex_atomic_h_
#define _regex_atomic_h_

#include "simple_regex.h"

#include <memory>
#include <vector>

#include <iostream>
namespace token_iterator {

// The Regex transition object corresponding to a single char.
template <typename Char>
struct singleton_regex_transition 
  : public regex_transition_cloner<
           singleton_regex_transition<Char>, Char
         > {
 public:
  using typename regex_state_transition<Char>::char_type;

  singleton_regex_transition(const char_type& t)
    : match {t},
      done {false} {}

  // This only matches when it was previously UNDECIDED and the 
  // character it receives (t), is a match.
  regex_state update(const char_type& t) override;
  regex_state initialize() override;
 private:
  char_type match;
  bool done;
};

template <typename Char>
regex_state singleton_regex_transition<Char>::update(const char_type& t) {
  if (!done && t == match) {
    done = true;
    return regex_state::MATCH;
  }
  return regex_state::MISMATCH;
}

template <typename Char>
regex_state singleton_regex_transition<Char>::initialize() {
  done = false;
  return regex_state::UNDECIDED;
}

template <typename Char>
simple_regex<Char> singleton_regex(const Char& ch) {
  regex_factory<singleton_regex_transition<Char>> fac;
  return fac.create(ch);
}


// The regex object corresponding to a predicate on a single char. 
// E.g. [:alpha:]
// In order for the initialize() method to be semantically correct, 
// the predicate Pred must be stateless.
template <typename Char>
class predicate_regex_transition 
  : public regex_transition_cloner<
             predicate_regex_transition<Char>, Char
           > {
 public:
  using typename regex_state_transition<Char>::char_type;
  using predicate_type = predicate_type_t<Char>;

  predicate_regex_transition(const predicate_type& p) 
    : pred {p},
      done {false} {}

  regex_state update(const Char& t) override;
  regex_state initialize() override;
 private:
  predicate_type pred;
  bool done;
};

template <typename Char>
regex_state 
predicate_regex_transition<Char>::update(const Char& t) {
  std::cout << "Computing update\n";
  if (!done && pred(t)) {
    done = true;
    return regex_state::MATCH;
  }
  return regex_state::MISMATCH;
}

template <typename Char>
regex_state predicate_regex_transition<Char>::initialize() {
  done = false;
  return regex_state::UNDECIDED;
}

template <typename Char>
simple_regex<Char> predicate_regex(const predicate_type_t<Char>& p) {
  regex_factory<predicate_regex_transition<Char>> fac;
  return fac.create(p);
}


template <typename Char>
struct always_true {
  bool operator()(const Char&) {return true;}
};

// This creates a regex that matches a '.'. The predicate
// always returns true.
template <typename Char>
simple_regex<Char> universal_singleton_regex() {
  return predicate_regex<Char>(always_true<Char>{});
}

template <typename Iterator>
class string_regex_transition 
  : public regex_transition_cloner<string_regex_transition<Iterator>, 
      char_type_t<Iterator>> {
 public:
  using char_type = char_type_t<Iterator>;
  using string_type = std::vector<char_type>;
  using size_type = typename string_type::size_type;

  string_regex_transition(Iterator begin, Iterator end)
    : string(begin, end),
      current {0} {}

  regex_state update(const char_type& t) override;
  regex_state initialize() override;
 private:
  string_type string;
  size_type current;
};  

template <typename Iterator> 
regex_state 
string_regex_transition<Iterator>::update(const char_type& t) {
  if (current != string.size() && 
      t == string[current]) {
    if (++current == string.size()) return regex_state::MATCH;
    return regex_state::UNDECIDED;
  }
  return regex_state::MISMATCH;
}

template <typename Iterator>
regex_state string_regex_transition<Iterator>::initialize() {
  current = 0;
  return string.empty()? regex_state::MATCH : regex_state::UNDECIDED;
}

template <typename Iterator>
simple_regex<char_type_t<Iterator>>
string_regex(Iterator begin, Iterator end) {
  regex_factory<string_regex_transition<Iterator>> fac;
  return fac.create(begin, end);
}


template <typename String>
simple_regex<typename String::value_type>
string_regex(const String& str) {
  return string_regex(str.begin(), str.end());
}

}//namespace token_iterator
#endif// _regex_atomic_h_
