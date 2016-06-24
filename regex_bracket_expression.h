#ifndef _regex_bracket_expression_h_
#define _regex_bracket_expression_h_

#include "regex_types.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <string>
#include <vector>
#include <utility>

namespace lex {
namespace detail {

// This is like assert, but it throws an exception.
inline void throw_if_not(bool p) {
  if (!p) {
    throw regex_error("Invalid regex: bad bracket");
  }
}

// This combines the element predicates of a matching list.
template <typename Char>
predicate_type_t<Char> matching_predicates(
    const std::vector<predicate_type_t<Char>>& predicates) {
  return [predicates] (const Char& ch) {
    return std::any_of(predicates.begin(), predicates.end(),
        [&ch](const auto& p) {return p(ch);});
  };
}

// This combines teh element predicates of a non-matching list.
template <typename Char>
predicate_type_t<Char> non_matching_predicates(
    const std::vector<predicate_type_t<Char>>& predicates) {
  return [predicates] (const Char& ch) {
    return std::none_of(predicates.begin(), predicates.end(),
        [&ch](const auto& p) {return p(ch);});
  };
}

template <typename Char>
predicate_type_t<Char> singleton_predicate(const Char& ch) {
  return [ch] (const Char& c) {
    return ch == c;
  };
}

template <typename Char>
predicate_type_t<Char> range_predicate(const Char& lower,
    const Char& upper) {
  return [lower, upper] (const Char& ch) {
    return lower <= ch && ch <= upper;
  };
}

template <typename Char>
predicate_type_t<Char>
built_in_predicate(const std::basic_string<Char>& name) {
  if (name == "alnum") {
    return [](const Char& ch) {return isalnum(ch);};
  }
  if (name == "alpha") {
    return [](const Char& ch) {return isalpha(ch);};
  }
  if (name == "lower") {
    return [](const Char& ch) {return islower(ch);};
  }
  if (name == "upper") {
    return [](const Char& ch) {return isupper(ch);};
  }
  if (name == "digit") {
    return [](const Char& ch) {return isdigit(ch);};
  }
  if (name == "xdigit") {
    return [](const Char& ch) {return isxdigit(ch);};
  }
  if (name == "space") {
    return [](const Char& ch) {return isspace(ch);};
  }
  if (name == "punct") {
    return [](const Char& ch) {return ispunct(ch);};
  }
  if (name == "graph") {
    return [](const Char& ch) {return isgraph(ch);};
  }
  if (name == "cntrl") {
    return [](const Char& ch) {return iscntrl(ch);};
  }
  if (name == "print") {
    return [](const Char& ch) {return isprint(ch);};
  }
  if (name == "blank") {
    return [](const Char& ch) {return isblank(ch);};
  }
  return [](const Char&) {return false;};
}

template <typename Iterator>
predicate_type_t<char_type_t<Iterator>>
bracket_class(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;

  throw_if_not(*begin == char_type('['));
  ++begin;
  throw_if_not(begin != end && *begin == char_type(':'));
  ++begin;
  std::basic_string<char_type> class_name;
  while (*begin != char_type(':')) {
    throw_if_not(begin != end);
    class_name.push_back(*begin);
    ++begin;
  }
  throw_if_not(begin != end && *begin == char_type(':'));
  ++begin;
  throw_if_not(begin != end && *begin == char_type(']'));
  ++begin;
  return built_in_predicate(class_name);
}

template <typename Iterator>
void
add_predicate(Iterator& begin, Iterator& end, 
    std::vector<predicate_type_t<char_type_t<Iterator>>>& preds) {
  using char_type = char_type_t<Iterator>;

  if (*begin == char_type('[')) {
    preds.push_back(bracket_class(begin, end));
    return;
  }
  auto start = *begin++;

  throw_if_not(begin != end);
  if (*begin == char_type('-')) {
    ++begin;
    throw_if_not(begin != end);
    if (*begin == char_type(']')) {
      preds.push_back(singleton_predicate(start));
      preds.push_back(singleton_predicate(char_type('-')));
      return;
    }
    if (*begin == char_type('[')) return;
    preds.push_back(range_predicate(start, *begin));
  } else {
    preds.push_back(singleton_predicate(start));
  }
  return;
}

// Post: begin points to the element past the closing bracket ']'.
template <typename Iterator>
std::vector<predicate_type_t<char_type_t<Iterator>>>
element_predicates(Iterator& begin, Iterator end) {
  using char_type = char_type_t<Iterator>;
  using predicate_type = predicate_type_t<char_type>;
  using std::move;

  throw_if_not(begin != end);

  std::vector<predicate_type> elements;
  auto ch = *begin;
  if (ch == char_type(']') || ch == char_type('-')) {
    auto pred = singleton_predicate(ch);
    elements.emplace_back(move(pred));
    ++begin;
    throw_if_not(begin != end);
  }

  while (*begin != char_type(']')) {
    throw_if_not(begin != end);
    add_predicate(begin, end, elements);
  }
  throw_if_not(*begin == char_type(']'));
  ++begin;
  return elements;
}

template <typename Iterator>
predicate_type_t<char_type_t<Iterator>>
bracket_expression(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  using std::move;

  assert(*begin == char_type('['));
  ++begin;
  throw_if_not(begin != end);

  bool matching {true};
  if (*begin == char_type('^')) {
    matching = false;
    ++begin;
  }

  std::vector<predicate_type_t<char_type>> elements =
    element_predicates(begin, end);
  auto result = matching? matching_predicates(elements) :
    non_matching_predicates(elements);

  return result;
}

}//namespace detail
}//namespace lex

#endif// _regex_bracket_expression_h_
