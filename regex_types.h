#ifndef _regex_types_h_
#define _regex_types_h_

#include <cstddef>
#include <functional>
#include <iterator>
#include <stdexcept>

namespace lex {

enum class match_state {MATCH, FINAL_MATCH, MISMATCH, UNDECIDED};

struct replication_data {
  replication_data(): lower {1}, upper {1} {}
  replication_data(std::size_t l, std::size_t u): lower {l}, upper {u} {}
  std::size_t lower;
  std::size_t upper;
};

struct regex_constants {
  enum error_type {
    error_none = 0,
    error_collate,
    error_ctype,
    error_escape,
    error_backref,
    error_brack,
    error_paren,
    error_brace,
    error_badbrace,
    error_range,
    error_space,
    error_badrepeat,
    error_complexity,
    error_stack,
    error_unsupported,
    error_badalt
  };

  enum syntax_option_type {
    icase = 1,
    nosubs = 2,
    optimize = 4,
    collate = 8,
    multiline = 16,
    ECMAScript = 32,
    basic = 64,
    extended = 128,
    awk = 256,
    grep = 512,
    egrep = 1024
  };

  enum context_type {
    expression,
    subexpression,
    brace,
    bracket
  };
};

inline constexpr regex_constants::syntax_option_type 
operator|(regex_constants::syntax_option_type left,
    regex_constants::syntax_option_type right) {
  return static_cast<regex_constants::syntax_option_type>(
      static_cast<unsigned int>(left) | static_cast<unsigned int>(right));
}
inline constexpr regex_constants::syntax_option_type 
operator&(regex_constants::syntax_option_type left,
    regex_constants::syntax_option_type right) {
  return static_cast<regex_constants::syntax_option_type>(
      static_cast<unsigned int>(left) & static_cast<unsigned int>(right));
}

template <typename CharIterator>
using value_type_t = 
  typename std::iterator_traits<CharIterator>::value_type;

template <typename Char>
using predicate_type_t = std::function<bool(const Char&)>;

struct regex_error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

}//namespace lex
#endif// _regex_types_h_
