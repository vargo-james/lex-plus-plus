#ifndef _token_stream_h_
#define _token_stream_h_

#include "expression_context.h"
#include "error_tracker.h"
#include "regex_token.h"
#include "simple_buffer.h"
#include "token_stream_impl.h"

namespace lex {

template <typename InputIt, typename Traits>
class token_stream : public error_tracker, public syntax_option {
 public:
  using value_type = typename token_stream_impl<InputIt,Traits>::value_type;
  using token = regex_token<value_type,Traits>;
  using syntax_option::flag_type;
  using error_tracker::error_type;
  using traits_type = Traits;

  token_stream(regex_range<InputIt>& src, flag_type f, error_type& er)
    : error_tracker(er),
      syntax_option(f),
      impl(src, f, er),
      source {src} {}

  token_stream(const token_stream& other)
    : error_tracker(other),
      syntax_option(other),
      impl(other.source, other.f, other.er),
      buffer {other.buffer},
      traits_ {other.traits_},
      source {other.source} {}

  bool get(token& out);
  bool putback(token tok) {return buffer.push(tok);}
  bool empty() const {return buffer.empty() && source.empty();}

  virtual ~token_stream() = default;
 private:
  token_stream_impl<InputIt,Traits> impl;
  simple_buffer<token> buffer;

  traits_type traits_;
  regex_range<InputIt>& source;

  virtual bool get_from_source(token& out) {return impl.get(out);} 
};

template <typename InputIt, typename Traits>
bool token_stream<InputIt,Traits>::get(token& out) {
  if (buffer.get(out)) {
    return true;
  }
  return get_from_source(out);
}

template <typename InputIt, typename Traits>
class replication_token_stream : public token_stream<InputIt,Traits> {
 public:
  using typename token_stream<InputIt,Traits>::value_type;
  using typename token_stream<InputIt,Traits>::token;
  using typename token_stream<InputIt,Traits>::flag_type;
  using typename token_stream<InputIt,Traits>::error_type;
  using typename token_stream<InputIt,Traits>::traits_type;

  using token_stream<InputIt,Traits>::token_stream;

 private:
  bool get_from_source(token& out) override;
};

template <typename InputIt, typename Traits>
bool replication_token_stream<InputIt,Traits>::get_from_source(token& out) {
  return false;
}

}//namespace lex
#endif// _token_stream_h_
