#ifndef _string_regex_atomic_h_
#define _string_regex_atomic_h_

#include "regex_alternation.h"
#include "regex_atomic.h"
#include "regex_concatenation.h"
#include "regex_replication.h"

#include "regex_bracket_expression.h"
#include "regex_reader_utilities.h"

#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace token_iterator {
// We need a function that takes a pair of CharIterators and returns
// a simple_regex<Char> object.
namespace detail {
template <typename Iterator>
simple_regex<char_type_t<Iterator>>
extended_reg_exp(Iterator& begin, Iterator& end);
}//namespace detail

template <typename Char>
simple_regex<Char> create_regex(const std::basic_string<Char>& regex_str) {
  auto begin = regex_str.begin();
  auto end = regex_str.end();
  auto regex = detail::extended_reg_exp(begin, end);
  if (begin != end) {
    throw regex_error("Invalid regex: could not read");
  }
  return regex;
}

namespace detail {
template <typename Iterator>
simple_regex<char_type_t<Iterator>>
read_parenthetical(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  ++begin;
  auto result = extended_reg_exp(begin, end);
  verify_closing(begin, end, char_type(')'));
  return result;
}

template <typename Iterator>
simple_regex<char_type_t<Iterator>>
one_char_or_coll_element_ERE(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  if (begin == end) return {};

  auto ch = *begin;
  switch (ch) {
  case char_type('['): {
    return predicate_regex(bracket_expression(begin, end));
  }
  case char_type('.'):
    ++begin;
    return universal_singleton_regex<char_type>();
  case char_type('\\'):
    return singleton_regex(quoted_char(begin, end));
  default:
    break;
  }
  return singleton_regex(ordinary_char(begin, end));
}

template <typename Iterator>
simple_regex<char_type_t<Iterator>>
single_element(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;

  if (*begin == char_type('(')) {
    return read_parenthetical(begin, end);
  }

  return one_char_or_coll_element_ERE(begin, end);
}

// This reads something like .{3}? or (abc){2}*{5,9}
template <typename Iterator>
simple_regex<char_type_t<Iterator>>
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

template <typename Iterator>
simple_regex<char_type_t<Iterator>>
ERE_branch(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  using regex_type = simple_regex<char_type>;
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

template <typename Iterator>
simple_regex<char_type_t<Iterator>>
extended_reg_exp(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  using regex_type = simple_regex<char_type>;
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
}//namespace token_iterator
#endif// _string_regex_atomic_h_
