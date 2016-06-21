#ifndef _regex_types_h_
#define _regex_types_h_

#include <cstddef>

namespace token_iterator {

enum class regex_state {MATCH, MISMATCH, UNDECIDED};

struct replication {
  replication(size_t l, size_t u): lower {l}, upper {u} {}
  size_t lower;
  size_t upper;
};

}//namespace token_iterator
#endif// _regex_types_h_
