#ifndef _regex_reader_utilities_h_
#define _regex_reader_utilities_h_

#include "regex_types.h"

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <limits>
#include <string>

namespace token_iterator {
namespace detail {
// Anchors are not supported and so are not considered special.
template <typename Char>
const std::basic_string<Char> special_characters() {
  return std::basic_string<Char>(".[()|*+?{\\");
}

template <typename Char>
bool is_special(Char ch) {
  const std::basic_string<Char> specials {".[()|*+?{\\"};
  return std::find(specials.begin(), specials.end(), ch) != specials.end();
}

template <typename Char>
regex_error invalid_close(const Char& close) {
  std::basic_string<Char> message {"Invalid regex: '"};
  message.push_back(close);
  message.append("' not found");
  return regex_error(message);
}

template <typename Iterator>
void verify_closing(Iterator& begin, Iterator& end,
    char_type_t<Iterator> close) {
  if (begin == end || *begin != close) {
    throw invalid_close(close); // throws an exception.
  }
  ++begin;
}

template <typename Iterator>
size_t read_number(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  std::basic_string<char_type> number;
  if (begin == end) {
    throw regex_error("Invalid regex: unable to read number");
  }
  while (begin != end && std::isdigit(*begin)) {
    number.push_back(*begin++);
  }
  return boost::lexical_cast<size_t>(number);
}

template <typename Iterator>
replication read_dupl_range(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  ++begin;

  auto lower = read_number(begin, end);
  if (begin == end) {
    throw regex_error("Invalid regex: unable to read range");
  }

  if (*begin == char_type('}')) {
      ++begin;
      return replication(lower, lower);
  }
  // The next line ensures that the next char is a comma.
  verify_closing(begin, end, char_type(','));

  if (*begin == char_type('}')) {
    ++begin;
    return replication(lower, std::numeric_limits<size_t>::max());
  }
  auto upper = read_number(begin, end);
  verify_closing(begin, end, char_type('}'));

  return replication(lower, upper);
}

template <typename Iterator>
replication ERE_dupl_symbol(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  if (begin == end) return replication(1,1);
  char_type ch = *begin;
  switch (ch) {
  case char_type('*'):
    ++begin;
    return replication(0, std::numeric_limits<size_t>::max());
  case char_type('+'):
    ++begin;
    return replication(1, std::numeric_limits<size_t>::max());
  case char_type('?'):
    ++begin;
    return replication(0, 1);
  case char_type('{'):
    return read_dupl_range(begin, end);
  default:
    break;
  }
  return replication(1,1);
}

template <typename Iterator>
char_type_t<Iterator>
quoted_char(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;

  assert(*begin++ == char_type('\\'));
  if (begin == end) {
    throw regex_error("Invalid regex: unable to read quoted char");
  }
  auto ch = *begin++;
  if (!is_special(ch)) {
    throw regex_error("Invalid regex: invalid special char");
  }
  return ch;
}

template <typename Iterator>
char_type_t<Iterator>
ordinary_char(Iterator& begin, Iterator& end) {
  auto ch = *begin;
  if (is_special(ch)) {
    return ch;
  }
  ++begin;
  return ch;
}

}//namespace detail
}//namespace token_iterator
#endif// _regex_reader_utilities_h_
