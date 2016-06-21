#ifndef _string_regex_atomic_h_
#define _string_regex_atomic_h_

#include "regex_atomic.h"
#include "regex_bracket_expression.h"
#include "regex_reader_utilities.h"

#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <iostream>

namespace token_iterator {
// We need a function that takes a pair of CharIterators and returns
// a simple_regex<Char> object.
namespace detail {
template <typename Iterator>
simple_regex<typename std::iterator_traits<Iterator>::value_type>
extended_reg_exp(Iterator& begin, Iterator& end);
}//namespace detail

template <typename Char>
simple_regex<Char> create_regex(const std::basic_string<Char>& regex_str) {
  auto begin = regex_str.begin();
  auto end = regex_str.end();
  auto regex = detail::extended_reg_exp(begin, end);
  if (begin != end) {
    throw std::runtime_error("Invalid regex: could not read");
  }
  return regex;
}

namespace detail {
template <typename Iterator>
simple_regex<typename std::iterator_traits<Iterator>::value_type>
read_parenthetical(Iterator& begin, Iterator& end) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;
  ++begin;
  auto result = extended_reg_exp(begin, end);
  std::cout << "CHAR: " << *begin << '\n';
  verify_closing(begin, end, char_type(')'));
  return result;
}

template <typename Iterator>
simple_regex<typename std::iterator_traits<Iterator>::value_type>
one_char_or_coll_element_ERE(Iterator& begin, Iterator& end) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;
  if (begin == end) return {};

  auto ch = *begin;
  switch (ch) {
  case char_type('['): {
    //auto predicate = bracket_expression(begin, end);
    return predicate_regex(bracket_expression(begin, end));
    break;
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
simple_regex<typename std::iterator_traits<Iterator>::value_type>
ERE_expression(Iterator& begin, Iterator& end) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;
  if (begin == end) return {};

  // We use a marker to check whether reads were successful.
  auto marker = begin;
  simple_regex<char_type> result;
  if (*begin == char_type('(')) {
    result = read_parenthetical(begin, end);
  } else {
    result = one_char_or_coll_element_ERE(begin, end);
  }
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
simple_regex<typename std::iterator_traits<Iterator>::value_type>
ERE_branch(Iterator& begin, Iterator& end) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;
  using std::move;

  if (begin == end) return {};

  std::vector<simple_regex<char_type>> expressions;
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
simple_regex<typename std::iterator_traits<Iterator>::value_type>
extended_reg_exp(Iterator& begin, Iterator& end) {
  using char_type = typename std::iterator_traits<Iterator>::value_type;
  using std::move;

  if (begin == end) return {};
  std::vector<simple_regex<char_type>> branches;

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

  std::cout << branches.size() << " Branches\n";
  if (branches.size() == 1) {
    return move(branches.front());
  }
  return matching_list(move(branches));
}
}//namespace detail
}//namespace token_iterator
#endif// _string_regex_atomic_h_
