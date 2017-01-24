/*
 * This class reads a regex from a range and a 
 * std::regex_constants::syntax_option_type variable.
 */

#ifndef _reader_h_
#define _reader_h_

#include "matcher/atomic.h"
#include "matcher/matcher.h"
#include "matcher/alternation.h"
#include "matcher/concatenation.h"
#include "matcher/replication.h"
#include "token_stream.h"

#include <iterator>
#include <regex>
#include <utility>
#include <vector>

namespace lex {


template <typename InputIt, typename Traits>
class reader {
 public:
  using flag_type = std::regex_constants::syntax_option_type;
  using iterator = InputIt;
  using value_type = typename std::iterator_traits<InputIt>::value_type;
  using matcher_type = matcher<value_type>;
  using stream_type = token_stream<InputIt,Traits>;
  using token = typename stream_type::token;

  reader(InputIt b, InputIt e, flag_type f)
    : ts(b,e,f),
      f_ {f} {}

  matcher_type read();
 private:
  token_stream<InputIt, Traits> ts;
  flag_type f_;
  int sub_depth {0};


  bool get_alternation(matcher_type& matcher);
  bool get_branch(matcher_type& matcher);
  bool get_expression(matcher_type& matcher);
  bool get_element(matcher_type& matcher);
  bool get_literals(matcher_type& matcher);
  bool get_bracket(matcher_type& matcher);
  bool get_sub_expression(matcher_type& matcher);
  bool get_replication(replication& rep);

  template <typename Compositor>
  matcher_type compose(std::vector<matcher_type>&& elements, 
      Compositor compositor);
};


template <typename InputIt, typename Traits>
typename reader<InputIt,Traits>::matcher_type
reader<InputIt,Traits>::read() {
  matcher_type matcher;
  get_alternation(matcher);
  return matcher;
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_alternation(matcher_type& matcher) {
  using std::move;
  if (ts.empty()) return false;

  token tok;
  std::vector<matcher_type> branches;
  for (matcher_type br; true; ) {
    if (!get_branch(br)) break;
    branches.push_back(move(br));
    if (!ts.get(tok)) break;
    if (tok.type != token_type::ALTERNATION) {
      ts.putback(tok);
      break;
    }
  }

  matcher = compose(std::move(branches), 
      [](auto&& v) {
        return lex::alternation(move(v));
      });
  return true;
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_branch(matcher_type& matcher) {
  using std::move;
  if (ts.empty()) return false;

  std::vector<matcher_type> expressions;
  for (matcher_type exp; get_expression(exp); expressions.push_back(exp)) {}

  matcher = compose(move(expressions),
      [](auto&& vec) {
        return lex::concatenate(move(vec));
      });
  return true;
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_expression(matcher_type& matcher) {
  using std::move;
  if (ts.empty()) return false;
  if (!get_element(matcher)) return false;

  for (replication rep; get_replication(rep);) {
    matcher = replicate(move(matcher), rep);
  }
  return true;
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_element(matcher_type& matcher) {
  token tok;
  if (!ts.get(tok)) return false;
  switch (tok.type) {
  case token_type::L_BRACKET:
    return get_bracket(matcher);
  case token_type::L_PAREN:
    if (!get_sub_expression(matcher)) {
      throw std::runtime_error("subexp");
    }
    return true;
  case token_type::ANY_CHAR:
    matcher = universal_singleton_matcher<value_type>();
    return true;
  case token_type::LITERAL:
    ts.putback(tok);
    return get_literals(matcher);
  case token_type::R_PAREN:
    if (sub_depth == 0) {
      tok = token(')', token_type::LITERAL);
      ts.putback(tok);
      return get_literals(matcher);
    } else {
      return false;
    }
  default:
    return false;
  }
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_literals(matcher_type& matcher) {
  using std::move;
  token tok;
  std::basic_string<value_type> str;
  for (value_type literal; ts.get(tok); ) {
    if (tok.type == token_type::R_PAREN && sub_depth == 0) {
      tok.type = token_type::LITERAL;
    }
    if (tok.type != token_type::LITERAL) {
      ts.putback(tok);
      break;
    }
    str.push_back(tok.value);
  }
  if (str.empty()) return false;
  matcher = string_matcher(move(str));
  return true;
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_replication(replication& rep) {
  return false;
  /*
  replication_reader rep_reader;
  return rep_reader.read(rep);
  using limits = std::numeric_limits<std::size_t>;

  if (ts.empty()) return false;
  token tok;
  ts.get(tok);
  if (tok.type == token_type::REPLICATION) {
    if (tok.value == value_type('*')) {
      rep = replication(0, limits::max());
    } else if (tok.value == value_type('+')) {
      rep = replication(1, limits::max());
    } else if (tok.value == value_type('?')) {
      rep = replication(0, 1);
    } else {
      return false;
    }
    return true;
  }

  if (tok.type != token_type::L_BRACE) {
    ts.putback(tok);
    return false;
  }

  return read_dupl_range(begin, end);
  */
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_sub_expression(matcher_type& matcher) {
  ++sub_depth;
  if (!get_alternation(matcher)) return false;
  token tok;
  if (!ts.get(tok) || tok.type != token_type::R_PAREN) {
    return false;
  }
  --sub_depth;
  return true;
}

template <typename InputIt, typename Traits>
bool reader<InputIt,Traits>::get_bracket(matcher_type& matcher) {
  return false;
  /*
  bracket_reader b_reader;
  return b_reader.read(matcher);
  */
}

template <typename InputIt, typename Traits>
template <typename Compositor>
typename reader<InputIt,Traits>::matcher_type
reader<InputIt,Traits>::compose(std::vector<matcher_type>&& elements,
    Compositor compositor) {
  using std::move;
  // The first alternative probably always indicates an error in the regex.
  if (elements.empty()) return {}; 
  if (elements.size() == 1) return move(elements.front()); 
  return compositor(move(elements));
}
  

}//namespace lex
#endif// _reader_
