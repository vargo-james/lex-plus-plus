#ifndef _lexer_alt_h_
#define _lexer_alt_h_

#include "lexer_impl.h"
#include "input_buffer.h"
#include "translator.h"

#include <iterator>
#include <memory>
#include <string>
#include <utility>

namespace lex {

template <typename InputIter>
class lexer {
 public:
  using buffer_type = input_buffer<InputIter>;
  using char_type = typename std::iterator_traits<InputIter>::value_type;
  using string_type = std::basic_string<char_type>;
  using function_type = std::function<void(const string_type&)>;

  lexer(InputIter begin, InputIter end, translator<lexer>* translator_ptr) 
    : buffer_p {std::make_unique<buffer_type>(begin, end)},
      trans_p {translator_ptr},
      lex_impl(buffer_p.get(), trans_p) {}

  lexer(const lexer&) = delete;
  lexer& operator=(const lexer&) = delete;
  lexer(lexer&&) = default;
  lexer& operator=(lexer&&) = default;
  ~lexer() = default;

  bool lex();
 private:
  std::unique_ptr<buffer_type> buffer_p;
  translator<lexer>* trans_p;
  lexer_impl<lexer> lex_impl;
};

template <typename InputIter>
bool lexer<InputIter>::lex() {
  auto result = lex_impl.do_lex();
  auto trans_it = result.second;
  // We check for a failed read.
  if (trans_it == trans_p->end()) return false;
  // We form a string from the matching input.
  auto buff_it = result.first;
  auto string_result = string_type(buffer_p->begin(), buff_it);
  // Now we call the function corresponding to the matched regex.
  (trans_it->second)(string_result);
  // We flush the processed input from the buffer.
  buffer_p->flush(buff_it);
  return true;
}

}//namespace lex
#endif// _lexer_alt_h_
