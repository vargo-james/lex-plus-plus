#ifndef _regex_types_h_
#define _regex_types_h_

#include <cstddef>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace lex {

enum class match_state {MATCH, FINAL_MATCH, MISMATCH, UNDECIDED};

struct replication {
  replication(size_t l, size_t u): lower {l}, upper {u} {}
  size_t lower;
  size_t upper;
};

template <typename CharIterator>
using char_type_t = 
  typename std::iterator_traits<CharIterator>::value_type;

template <typename Iterator, typename Char>
struct enable_iterator {
  using type = 
    std::enable_if_t<
      std::is_same<Char, char_type_t<Iterator>>::value
    >;
};

template <typename Iterator, typename Char>
using enable_iterator_t = typename enable_iterator<Iterator,Char>::type;

template <typename Container, typename T>
struct enable_container {
  using type = 
    std::enable_if<
      std::is_same<T, typename Container::value_type>::value
    >;
};

template <typename Container, typename T>
using enable_container_t = typename enable_container<Container, T>::type;
     
template <typename Char>
using predicate_type_t = std::function<bool(const Char&)>;

struct regex_error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

}//namespace lex
#endif// _regex_types_h_
