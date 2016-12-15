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
  using char_type = typename iterator::value_type;

  template <typename Container>
  explicit range(const Container& c)
    : begin(std::begin(c)),
      end(std::end(c)) {}

  range(const range& other) = default;
  range& operator=(const range& other) = default;
  range(range&& other) = default;
  range& operator=(range&& other) = default;

  range(const iterator& b, const iterator& e): begin {b}, end {e} {}
  
  char_type current() const {return *begin;}
  range& advance();

  // Calls advance and throws if the range is now empty.
  range& checked_advance();

  bool current_is(const char_type& ch) const {return current() == ch;}
  bool bracket_open() const {return current_is('[');}
  bool bracket_close() const {return current_is(']');}

  bool empty() const {return begin == end;}

  void check(bool b, const std::string& msg = {});
  void check(const char_type& ch, const std::string& msg = {}) {
    check(current_is(ch), msg);
  }
  void check_bracket_open() {
    check(char_type('['), "check_bracket_open");
  }
  void check_bracket_close() {
    check(char_type(']'), "check_bracket_close");
  }

  // DEBUG
  iterator& get_begin() {return begin;}
  iterator& get_end() {return end;}
 private:
  iterator begin;
  iterator end;
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
