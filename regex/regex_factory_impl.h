/*
 * Questions:
 *  extended_reg_exp(): 
 *    Do we need emplace_back?
 *    Do we need to std::move brances.front()?
 *  ERE_branch
 *    Do we need emplace_back?
 */
#ifndef _regex_factory_impl_h_
#define _regex_factory_impl_h_

#include "matcher/alternation.h"
#include "matcher/atomic.h"
#include "matcher/concatenation.h"
#include "matcher/replication.h"

#include "regex_bracket.h"

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <iterator>
#include <limits>
#include <regex>
#include <string>
#include <utility>
#include <vector>

namespace lex {
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
    throw invalid_close(close);
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

  assert(*begin == char_type('\\'));
  ++begin;

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
// We need a function that takes a pair of CharIterators and returns
// a matcher<Char> object.
namespace detail {
template <typename Iterator, typename Traits = 
  std::regex_traits<char_type_t<Iterator>>>
matcher<char_type_t<Iterator>>
extended_reg_exp(Iterator& begin, Iterator& end);

template <typename Iterator>
matcher<char_type_t<Iterator>>
read_parenthetical(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  ++begin;
  auto result = extended_reg_exp(begin, end);
  verify_closing(begin, end, char_type(')'));
  return result;
}

template <typename Iterator>
matcher<char_type_t<Iterator>>
one_char_or_coll_element_ERE(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  if (begin == end) return {};

  auto ch = *begin;
  switch (ch) {
  case char_type('['): {
    return predicate_matcher(bracket_expression(begin, end));
  }
  case char_type('.'):
    ++begin;
    return universal_singleton_matcher<char_type>();
  case char_type('\\'):
    return singleton_matcher(quoted_char(begin, end));
  default:
    break;
  }
  return singleton_matcher(ordinary_char(begin, end));
}

// This function reads a single regular expression element without the need
// to consider replication, concatenation, or alternation. There are two cases.
// The first case is a parenthetical expression. In that case, the contents
// inside the parenthesis can be any regular expression, so we must delegate
// to the top level reader.
// In the second case, we have a single character or collating element.
template <typename Iterator>
matcher<char_type_t<Iterator>>
single_element(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;

  if (*begin == char_type('(')) {
    return read_parenthetical(begin, end);
  }

  return one_char_or_coll_element_ERE(begin, end);
}

// This reads something like .{3}? or (abc){2}*{5,9}. That is,
// a single element which is replicated one or more times.
template <typename Iterator>
matcher<char_type_t<Iterator>>
ERE_expression(Iterator& begin, Iterator& end) {
  if (begin == end) return {};

  auto marker = begin;
  auto result = single_element(begin, end);

  // Now we loop while reading all replication symbols.
  if (marker == begin) return {};
  marker = begin;
  auto replication = ERE_dupl_symbol(begin, end); 
  while (begin != marker) {
    result = replicate(std::move(result), replication);
    marker = begin;
    replication = ERE_dupl_symbol(begin, end);
  }
  return result;
}

// This function reads an individual branch, thus the alternation operator |
// need not be considered. Instead this reads a sequence of composable
// regular expressions and then concatenates them. The individual units
// are read using the ERE_expression function.
template <typename Iterator>
matcher<char_type_t<Iterator>>
ERE_branch(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  using regex_type = matcher<char_type>;
  using std::move;

  if (begin == end) return {};

  std::vector<regex_type> expressions;
  while (true) {
    // A marker is used to check whether a read took place.
    auto marker = begin;
    auto exp = ERE_expression(begin, end);
    if (begin == marker) break; // Unsucessful read.
    expressions.emplace_back(move(exp));
  }

  return concatenate(move(expressions));
}

// This function reads a regular expression and returns the corresponding
// matcher object.
// Specifically, it calls a branch reader and combines the branches
// using matcher alternation. In other words it does the work of parsing
// regex = branch_1|branch_2|...|branch_k
template <typename Iterator, typename Traits = 
  std::regex_traits<char_type_t<Iterator>>>
matcher<char_type_t<Iterator>>
extended_reg_exp(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  using regex_type = matcher<char_type>;
  using std::move;

  if (begin == end) return {};
  std::vector<regex_type> branches;

  char_type ch {'\0'};
  do {
    if (ch) ++begin; // This happens on all iterations after the first.
    // Successful reads move the begin iterator. We set a marker to test
    // that.
    auto marker = begin; 
    auto branch = ERE_branch(begin, end);
    if (begin == marker) break; // A branch could not be read.
    branches.emplace_back(move(branch));
    if (begin == end) break;  // Nothing more can be read.
    ch = *begin;
  } while (ch == char_type('|'));

  if (branches.size() == 1) {
    return move(branches.front());
  }
  return alternation(move(branches));
}
}//namespace detail
}//namespace lex
#endif// _regex_factory_impl_h_
