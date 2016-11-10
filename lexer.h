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

template <typename InputIter, 
          typename Traits = std::regex_traits<char_type_t<InputIter>>
         >
class lexer {
 public:
  using buffer_type = input_buffer<InputIter>;
  using char_type = char_type_t<InputIter>;
  using string_type = typename Traits::string_type;
  using function_type = std::function<void(const string_type&)>;
  using translator_type = translator<lexer>;
  using translator_item = std::pair<string_type, function_type>;

  lexer(InputIter begin, InputIter end, 
        const translator_type& translator) 
    : buffer_p {std::make_unique<buffer_type>(begin, end)},
      trans {translator},
      lex_impl(buffer_p.get(), trans) {}

  lexer(const lexer&) = delete;
  lexer& operator=(const lexer&) = delete;
  lexer(lexer&&) = default;
  lexer& operator=(lexer&&) = default;
  ~lexer() = default;

  bool lex();

  template <typename Container, 
            typename = enable_container_t<Container, translator_item>>
  void set_translator(Container&& c) {
    trans = translator_type(c.begin(), c.end());
  }

 private:
  std::unique_ptr<buffer_type> buffer_p;
  translator_type trans;
  lexer_impl<lexer> lex_impl;
};

template <typename InputIter, typename Traits>
bool lexer<InputIter,Traits>::lex() {
  auto result = lex_impl.do_lex();
  auto trans_it = result.second;
  // We check for a failed read.
  if (trans_it == trans.end()) return false;
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
