#ifndef _token_stream_h_
#define _token_stream_h_

#include "regex_range.h"
#include "regex_context.h"
#include "regex_token.h"
#include "simple_buffer.h"

#include <regex>

namespace lex {


template <typename InputIt>
class token_stream {
 public:
  using flag_type = std::regex_constants::syntax_option_type;
  using iterator = InputIt;
  using value_type = typename std::iterator_traits<InputIt>::value_type;
  using token_type = regex_token<value_type>;
  using buffer_type = simple_buffer<token_type>;

  token_stream(InputIt b, InputIt e, flag_type f)
    : flag {f},
      range(b,e) {}

  bool get(token_type& out);
  bool putback(token_type tok) {return buffer.set(tok);}

  bool empty() const {return !range.empty();}
 private:
  flag_type flag;
  regex_range<iterator> range;
  buffer_type buffer;
  regex_context context;

  void get_from_range(token_type& out);
  void default_get(token_type& out);
  void replication_get(token_type& out);
  void subexpr_get(token_type& out);
  void bracket_get(token_type& out);
  void collate_get(token_type& out);
  void equiv_get(token_type& out);
  void class_get(token_type& out);
};

template <typename InputIt>
bool token_stream<InputIt>::get(token_type& out) {
  if (buffer.get(out)) {
    return true;
  }
  
  if (range.empty()) return false;

  get_from_range(out);
  context.update(out);
  return true;
}

template <typename InputIt>
void token_stream<InputIt>::get_from_range(token_type& out) {
  switch (context.get()) {
  case regex_context::DEFAULT:
    default_get(out);
    break;
  case regex_context::REPLICATION:
    replication_get(out);
    break;
  case regex_context::SUBEXPR:
    subexpr_get(out);
    break;
  case regex_context::BRACKET:
    bracket_get(out);
    break;
  case regex_context::COLLATE:
    collate_get(out);
    break;
  case regex_context::EQUIV:
    equiv_get(out);
    break;
  case regex_context::CLASS:
    class_get(out);
    break;
  default:
    break;
  }
}

}//namespace lex
#endif// _token_stream_h_
