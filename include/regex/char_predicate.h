/*
 * There are several important types of character predicates.
 *
 * 1. Literal: [ch](const auto& other) {return ch == other;}
 * 2. icase: [ch](const auto& other) {return lower(ch) == lower(other);}
 * 3. class: [class] (const auto& ch) {return class.contains(ch);}
 * 4. range: [l,h] (const auto& ch) {return l <= ch && ch <= h;}
 * 5. coll_el: [el] (const auto& ch) {return coll(ch) == el;}
 *
 */

#ifndef _char_predicate_h_
#define _char_predicate_h_

#include "regex_types.h"

// This is like assert, but it throws an exception.
inline void throw_if_not(bool p) {
  if (!p) {
    throw regex_error("Invalid regex: bad bracket");
  }
}

template <typename Iterator>
void advance(Iterator& begin, Iterator& end) {
  ++begin;
  throw_if_not(begin != end);
}

template <typename Char, typename Traits>
class char_predicate {
 public:
  using char_type = Char;
  using predicate_type = predicate_type_t<Char>;

  predicate_type read(Iterator& begin, Iterator& end);
 private:
};

template <typename Char, typename Traits>
template <typename Iterator>
char_predicate<Char, Traits>::predicate_type
char_predicate<Char, Traits>::read(Iterator& begin, Iterator& end) { 
  // Special classes begin with [. [= [:.
  if (is_special(begin, end)) {
    return special_class(begin, end);
  }

  auto ch = *begin;
  if (ch != '[') {
    advance(begin, end);
    return class_reader(begin, end);
  }
  
}
#endif// _char_predicate_h_
