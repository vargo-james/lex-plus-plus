/*
 *  What about [^]. Should initialize check that?
 *
 *  read_next_element needs to be fixed.
 */
#ifndef _regex_bracket_expression_h_
#define _regex_bracket_expression_h_

#include "bracket_list.h"
#include "range.h"
#include "regex_types.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <regex>
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

// Question: What does [a-[... mean? It corresponds to a puzzling branch in 
// this function.
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
    elements.push_back(move(pred));
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

// This combines the element predicates of a matching list.
template <typename Char>
predicate_type_t<Char> matching_predicates(
    const std::vector<predicate_type_t<Char>>& predicates) {
  return [predicates] (const Char& ch) {
    return std::any_of(predicates.begin(), predicates.end(),
        [&ch](const auto& p) {return p(ch);});
  };
}

// This combines the element predicates of a non-matching list.
template <typename Char>
predicate_type_t<Char> non_matching_predicates(
    const std::vector<predicate_type_t<Char>>& predicates) {
  return [predicates] (const Char& ch) {
    return std::none_of(predicates.begin(), predicates.end(),
        [&ch](const auto& p) {return p(ch);});
  };
}

template <typename Iterator>
predicate_type_t<char_type_t<Iterator>>
bracket_expression(Iterator& begin, Iterator& end) {
  using char_type = char_type_t<Iterator>;
  using std::move;

  throw_if_not(*begin == char_type('['));
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

template <typename Char, typename Traits = std::regex_traits<Char>>
class bracket_reader {
 public:
  using char_type = Char;
  using string_type = typename Traits::string_type;
  using iterator = typename string_type::const_iterator;
  using range_type = range<iterator>;
  using element_type = predicate_type_t<char_type>;

  // Pre: begin points to '['.
  // Post: begin points past ']'.
  // Throws regex_error if a formatting error is encountered.
  element_type read(range_type& r);

 private:
  bracket_list<element_type> elements;

  // This method is the first to be called. 
  //   It identifies the correct value of matching_.
  //   It advances begin to the first character after '[' or '^' 
  //    in the non-matching case.
  //   It throws if a formatting error is found.
  void initialize(range_type& r);

  void read_elements(range_type& r);

  void read_next_element(range_type& r);

  void clear() {elements.clear();}
  // This combines all the elements that have been read.
  element_type combine_elements() const;
};

template <typename Char, typename Traits>
typename bracket_reader<Char, Traits>::element_type
bracket_reader<Char, Traits>::read(range_type& r) {
  initialize(r);
  read_elements(r);
  auto p = combine_elements();
  clear();
  return p;
}

template <typename Char, typename Traits>
void bracket_reader<Char, Traits>::initialize(range_type& r) {
  r.check_bracket_open();
  r.checked_advance();

  if (r.current_is(char_type('^'))) {
    elements.set_matching(false);
    r.checked_advance();
  }
  return;
}

template <typename Char, typename Traits>
void bracket_reader<Char, Traits>::read_elements(range_type& r) {
  using std::move;

  if (r.bracket_close()) {
    elements.add_element(singleton_predicate(r.current()));
    r.checked_advance();
  }
  if (r.current_is(char_type('-'))) {
    elements.add_element(singleton_predicate(r.current()));
    r.checked_advance();
  }

  while (!r.bracket_close()) {
    read_next_element(r);
  }
  r.check_bracket_close();
  r.advance();
}

// This currently depends on the free functions:
//   bracket_class - to read things like [:alpha:]
//   singleton_predicate - for simple elements
//   range_predicate - for ranges such as a-z
template <typename Char, typename Traits>
void bracket_reader<Char, Traits>::read_next_element(range_type& r) {

  if (r.bracket_open()) {
    auto begin = r.get_begin();
    auto end = r.get_end();
    elements.add_element(bracket_class(begin, end));
    return;
  }

  auto lower = r.current();
  r.checked_advance();

  if (r.current_is(char_type('-'))) {
    r.checked_advance();
    if (r.bracket_close()) {
      elements.add_element(singleton_predicate(lower));
      elements.add_element(singleton_predicate(char_type('-')));
      return;
    }
    // WHAT IS THIS?? THIS IS CURRENTLY AN ERROR.
    if (r.bracket_open()) {
      return;
    }
    elements.add_element(range_predicate(lower, r.current()));
  } else {
    elements.add_element(singleton_predicate(lower));
  }
}

template <typename Char, typename Traits>
typename bracket_reader<Char, Traits>::element_type 
bracket_reader<Char, Traits>::combine_elements() const {
  return elements.combine();
}

}//namespace detail
}//namespace lex

#endif// _regex_bracket_expression_h_
