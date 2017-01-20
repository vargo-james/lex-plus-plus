/*
 * This is the range class file.
 *
 * This class has been developed for the purpose of parsing a regex.
 * For example, if the regex is (ab){1-4}3? Then the regex string object 
 * is used to initialize two iterators begin, end that represent the range.
 * We then use the range methods as a convenience in parsing the string.
 * At any time, the range represented by the range object is the part of 
 * the string that has not yet been parsed.
 *
 * Types:
 *  value_type is the the value_type of the regex string.
 *
 * Methods:
 *  We read the string using current() and advance(). We can check whether
 *  we have read the entire string by calling empty(). There are additional
 *  methods that are defined for the convenience of parsing. Essentially
 *  they are useful for verifying the correct formatting of the input string.
 */

#ifndef _range_h_
#define _range_h_

#include "regex_types.h"

#include <iterator>
#include <string>

namespace lex {

template <typename Iterator>
class range {
 public:
  using iterator = Iterator;
  using value_type = typename std::iterator_traits<iterator>::value_type;

  template <typename Container>
  explicit range(const Container& c)
    : begin(std::begin(c)),
      end(std::end(c)),
      mark(std::begin(c)) {}

  range(Iterator first, Iterator last)
    : begin {first}, 
      end {last}, 
      mark {first} {}

  range(const range& other) = default;
  range& operator=(const range& other) = default;
  range(range&& other) = default;
  range& operator=(range&& other) = default;

  value_type current() const {return *begin;}
  range& advance();

  // Calls advance and throws if the range is now empty.
  range& checked_advance();

  bool current_is(const value_type& ch) const {return current() == ch;}
  bool bracket_open() const {return current_is('[');}
  bool bracket_close() const {return current_is(']');}

  bool empty() const {return begin == end;}

  void set_mark() {mark = begin;}
  bool has_advanced() const {return mark != begin;}

  void check(bool b, const std::string& msg = {});
  void check(const value_type& ch, const std::string& msg = {}) {
    check(current_is(ch), msg);
  }
  void check_bracket_open() {
    check(value_type('['), "check_bracket_open");
  }
  void check_bracket_close() {
    check(value_type(']'), "check_bracket_close");
  }

  // DEBUG
  iterator& get_begin() {return begin;}
  iterator& get_end() {return end;}
 private:
  iterator begin;
  iterator end;
  iterator mark;
};

template <typename Iterator>
range<Iterator>& range<Iterator>::advance() {
  ++begin;
  return *this;
}

template <typename Iterator>
range<Iterator>& range<Iterator>::checked_advance() {
  ++begin;
  check(!empty(), "checked_advance");
  return *this;
}

template <typename Iterator>
void range<Iterator>::check(bool b, const std::string& msg) {
  if (!b) {
    throw regex_error(msg);
  }
}

}//namespace
#endif// _range_h_
