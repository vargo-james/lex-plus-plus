#ifndef _regex_reader_utilities_h_
#define _regex_reader_utilities_h_

#include "regex_replication.h"

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>

namespace token_iterator {
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

template <typename Iterator>
void verify_closing(Iterator& begin, Iterator& end,
    typename std::iterator_traits<Iterator>::value_type close) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;
  if (begin == end || *begin != close) {
    std::basic_string<char_type> message {"Invalid regex: '"};
    message.push_back(close);
    message.append("' not found");
    throw std::runtime_error(message);
  }
  ++begin;
}

template <typename Iterator>
size_t read_number(Iterator& begin, Iterator& end) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;
  std::basic_string<char_type> number;
  if (begin == end) {
    throw std::runtime_error("Invalid regex: unable to read number");
  }
  while (begin != end && std::isdigit(*begin)) {
    number.push_back(*begin++);
  }
  return boost::lexical_cast<size_t>(number);
}

template <typename Iterator>
replicator read_dupl_range(Iterator& begin, Iterator& end) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;
  ++begin;

  auto lower = read_number(begin, end);
  if (begin == end) {
    throw std::runtime_error("Invalid regex: unable to read range");
  }

  if (*begin == char_type('}')) {
      ++begin;
      return replicator(lower, lower);
  }
  // The next line ensures that the next char is a comma.
  verify_closing(begin, end, char_type(','));

  if (*begin == char_type('}')) {
    ++begin;
    return replicator(lower, std::numeric_limits<size_t>::max());
  }
  auto upper = read_number(begin, end);
  verify_closing(begin, end, char_type('}'));

  return replicator(lower, upper);
}

template <typename Iterator>
replicator ERE_dupl_symbol(Iterator& begin, Iterator& end) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;
  if (begin == end) return replicator(1,1);
  char_type ch = *begin;
  switch (ch) {
  case char_type('*'):
    ++begin;
    return replicator(0, std::numeric_limits<size_t>::max());
  case char_type('+'):
    ++begin;
    return replicator(1, std::numeric_limits<size_t>::max());
  case char_type('?'):
    ++begin;
    return replicator(0, 1);
  case char_type('{'):
    return read_dupl_range(begin, end);
  default:
    break;
  }
  return replicator(1,1);
}

template <typename Iterator>
typename std::iterator_traits<Iterator>::value_type
quoted_char(Iterator& begin, Iterator& end) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;

  assert(*begin++ == char_type('\\'));
  if (begin == end) {
    throw std::runtime_error("Invalid regex: unable to read quoted char");
  }
  auto ch = *begin++;
  if (!is_special(ch)) {
    throw std::runtime_error("Invalid regex: invalid special char");
  }
  return ch;
}

template <typename Iterator>
typename std::iterator_traits<Iterator>::value_type
ordinary_char(Iterator& begin, Iterator& end) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;
  auto ch = *begin;
  if (is_special(ch)) {
    return char_type('\0');
  }
  ++begin;
  return ch;
}

}//namespace token_iterator
#endif// _regex_reader_utilities_h_
