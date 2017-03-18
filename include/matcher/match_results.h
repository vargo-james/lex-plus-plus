#ifndef _regex_matcher_match_results_h_
#define _regex_matcher_match_results_h_

#include "data_structures/forward_iterator.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace lex {

template <typename String>
class match_results {
 public:
  using string_type = String;
  using value_type = typename String::value_type;
  using submatch = std::pair<std::size_t, std::size_t>;
  using size_type = typename std::vector<submatch>::size_type;
 
  void append(forward_iterator<value_type, const value_type&> b, 
      forward_iterator<value_type, const value_type&> e) {str.append(b,e);}

  void add_submatch(std::size_t sub_b, std::size_t sub_e) {
    submatches.push_back({sub_b, sub_e});
  }

  const submatch& operator[](size_type index) {
    return index? submatches[index - 1] : submatch{0, str.size()};
  }

  const String& full_results() {return str;}
  void clear() {str.clear(); submatches.clear();}
 private:
  using input_iterator = forward_iterator<value_type, const value_type&>;
  using buffer_type = input_buffer<input_iterator>;

  buffer_type* buffer;
  std::vector<submatch> submatches;
};
}//namespace lex
#endif// _regex_matcher_match_results_h_
