#ifndef _lexer_impl_h_
#define _lexer_impl_h_

#include "buffer_iterator.h"
#include "matcher/matcher.h"
#include "translator.h"

#include <cassert>
#include <string>
#include <utility>

namespace lex {

template <typename L>
class lexer_impl {
 public:
  using buffer_type = typename L::buffer_type;
  using function_type = typename L::function_type;
  using char_type = typename buffer_type::value_type;
  using regex_type = matcher<char_type>;
  using translator_type = translator<L>;

  lexer_impl(buffer_type* buff_ptr, 
      translator_type* ptr) 
    : buffer {buff_ptr},
      trans {ptr} {}

  lexer_impl(const lexer_impl&) = default;
  lexer_impl& operator=(const lexer_impl&) = default;
  lexer_impl(lexer_impl&&) = default;
  lexer_impl& operator=(lexer_impl&&) = default;
  ~lexer_impl() = default;

  auto do_lex();
 private:
  buffer_type* buffer;
  translator_type* trans;

  buffer_iterator<buffer_type> regex_match(regex_type& regex); 
};

template <typename L>
buffer_iterator<typename L::buffer_type> 
lexer_impl<L>::regex_match(regex_type& regex) {
  assert(regex.state() != match_state::MISMATCH);

  auto accepted = buffer->begin();
  bool loop_done {false};
  for (auto seek = buffer->begin(); !loop_done && seek != buffer->end();) {
    regex.update(*seek);
    ++seek;
    switch (regex.state()) {
    case match_state::MATCH:
      accepted = seek;
      break;
    case match_state::FINAL_MATCH:
      accepted = seek;
      loop_done = true;
      break;
    case match_state::MISMATCH:
      loop_done = true;
      break;
    case match_state::UNDECIDED:
      break;
    }
  }
  // The regex needs to be reinitialized to be used again later.
  regex.initialize();
  return accepted;
}

template <typename L>
//typename lexer_impl<L>::lex_result lexer_impl<L>::do_lex() {
auto lexer_impl<L>::do_lex() {
  // The regex match will be the range [begin, accepted).
  auto it = trans->begin();
  while (it != trans->end()) {
    auto regex = it->first;
    auto accepted = regex_match(regex);
    if (accepted != buffer->begin()) {
      return std::make_pair(accepted, it);
    }
    ++it;
  }
  return std::make_pair(buffer->begin(), trans->end());
}

}//namespace lex
#endif// _lexer_impl_h_
