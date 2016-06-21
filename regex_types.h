#ifndef _regex_types_h_
#define _regex_types_h_

#include <cstddef>
#include <functional>
#include <iterator>

namespace token_iterator {

enum class regex_state {MATCH, MISMATCH, UNDECIDED};

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

}//namespace token_iterator
#endif// _regex_types_h_
