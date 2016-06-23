#ifndef _lexer_h_
#define _lexer_h_

#include "input_buffer.h"
#include "simple_regex.h"
#include "translator.h"

#include <cassert>
#include <iterator>
#include <memory>
#include <string>

namespace token_iterator {

// F is callable on a pair of 
template <typename InputIter, typename F>
class lexer {
 public:
  using buffer_type = input_buffer<InputIter>;
  using buffer_iterator_type = buffer_iterator<buffer_type>;
  using input_buffer_ptr = std::shared_ptr<buffer_type>;
  using char_type = typename std::iterator_traits<InputIter>::value_type;
  using regex_type = simple_regex<char_type>;
  using function_type = F;
  using translator_ptr = translator<char_type, function_type>*;

  lexer(InputIter begin, InputIter end, translator_ptr ptr) 
    : buffer {std::make_shared<buffer_type>(begin, end)},
      trans {ptr} {}

  lexer(const lexer&) = default;
  lexer& operator=(const lexer&) = default;
  lexer(lexer&&) = default;
  lexer& operator=(lexer&&) = default;

  ~lexer() = default;
  bool get();
 protected:
  input_buffer_ptr buffer;
  translator_ptr trans;

  buffer_iterator_type regex_match(regex_type& regex); 
 private:
  void call(const F& f, buffer_iterator_type b, buffer_iterator_type e);
};

template <typename InputIter, typename F>
buffer_iterator<input_buffer<InputIter>> 
lexer<InputIter,F>::regex_match(regex_type& regex) {
  assert(regex.state() != regex_state::MISMATCH);

  auto accepted = buffer->begin();
  bool loop_done {false};
  for (auto seek = buffer->begin(); !loop_done && seek != buffer->end();) {
    regex.update(*seek);
    ++seek;
    switch (regex.state()) {
    case regex_state::MATCH:
      accepted = seek;
      break;
    case regex_state::FINAL_MATCH:
      accepted = seek;
      loop_done = true;
      break;
    case regex_state::MISMATCH:
      loop_done = true;
      break;
    case regex_state::UNDECIDED:
      break;
    }
  }
  // The regex needs to be reinitialized to be used again later.
  regex.initialize();
  return accepted;
}

template <typename InputIter, typename F>
bool lexer<InputIter,F>::get() {
  // The regex match will be the range [begin, accepted).
  auto begin = buffer->begin();
  for (auto& pattern : *trans) {
    auto accepted = regex_match(pattern.first);
    // If this regex matches some input, then we are done.
    if (accepted != begin) {
      // Now we call the function on the range.
      call(pattern.second, begin, accepted);
      // We flush the processed input from the buffer.
      buffer->flush(accepted);
      return true;
    }
  }
  return false;
}

template <typename InputIter, typename F>
void lexer<InputIter,F>::call(const F& f, buffer_iterator_type b, 
    buffer_iterator_type e) {
  std::basic_string<char_type> str(b,e);
  f(std::move(str));
}

}//namespace token_iterator
#endif// _lexer_h_
