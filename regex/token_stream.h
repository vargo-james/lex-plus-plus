#ifndef _token_stream_h_
#define _token_stream_h_

#include "regex_token.h"
#include "simple_buffer.h"
#include "token_stream_impl.h"

namespace lex {

template <typename InputIt, typename Traits>
class token_stream {
 public:
  using value_type = typename token_stream_impl<InputIt,Traits>::value_type;
  using token = regex_token<value_type,Traits>;
  using flag_type = typename token_stream_impl<InputIt,Traits>::flag_type;

  token_stream(InputIt b, InputIt e, flag_type f)
    : impl(b,e,f) {} 

  bool get(token& out);
  bool putback(token tok) {return buffer.push(tok);}
  bool empty() const {return buffer.empty() && source_empty();}

 private:
  simple_buffer<token> buffer;
  token_stream_impl<InputIt,Traits> impl;

  bool source_empty() const {return impl.empty();}
  void get_from_source(token& out) {impl.get(out);} 
};

template <typename InputIt, typename Traits>
bool token_stream<InputIt,Traits>::get(token& out) {
  if (buffer.get(out)) {
    return true;
  }
  if (source_empty()) return false;
  get_from_source(out);
  return true;
}

}//namespace lex
#endif// _token_stream_h_
