#ifndef _regex_types_h_
#define _regex_types_h_

#include <cstddef>
#include <functional>
#include <iterator>
#include <stdexcept>

namespace lex {

enum class match_state {MATCH, FINAL_MATCH, MISMATCH, UNDECIDED};

struct replication {
  replication(size_t l, size_t u): lower {l}, upper {u} {}
  size_t lower;
  size_t upper;
};

template <typename StringIterator>
using char_type_t = 
  typename std::iterator_traits<StringIterator>::value_type;

template <typename Char>
using predicate_type_t = std::function<bool(const Char&)>;

struct regex_error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

}//namespace lex
#endif// _regex_types_h_
