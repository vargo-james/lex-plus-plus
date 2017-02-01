#ifndef _replication_reader_h_
#define _replication_reader_h_

#include "error_tracker.h"
#include "regex_types.h"
#include "token_stream.h"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>

namespace lex {

template <typename InputIt, typename Traits>
class replication_reader : public error_tracker {
 public:
  using error_tracker::error_type;
  using value_type = typename std::iterator_traits<InputIt>::value_type;
  using token = regex_token<value_type,Traits>;

  replication_reader(Traits& t, token_stream<InputIt,Traits>& stream, 
      error_type& e)
    : error_tracker(e),
      traits_ {t},
      ts {stream} {}

  bool read(replication_data& data);

 private:
  Traits& traits_;
  token_stream<InputIt, Traits>& ts;

  //void set_error(error_type et) {ts.set_error(et);}
  bool read_rep_symbol(replication_data& data, value_type ch);
  bool read_braced_range(replication_data& data);
  bool read_number(std::size_t& value);
  bool read_upper(std::size_t& upper);
  bool verify_closing(std::size_t lower, std::size_t upper);
  static const int radix = 10;
};

template <typename InputIt, typename Traits>
bool replication_reader<InputIt,Traits>::read(replication_data& data) {
  token tok;
  if (!ts.get(tok)) return false;
  
  if (tok.type == token_type::REPLICATION) {
    return read_rep_symbol(data, tok.ch);
  }

  ts.putback(tok);
  if (tok.type != token_type::L_BRACE) return false;

  return read_braced_range(data);
}

template <typename InputIt, typename Traits>
bool 
replication_reader<InputIt,Traits>::read_rep_symbol(replication_data& data, 
    value_type symbol) {
  using limits = std::numeric_limits<std::size_t>;

  switch (symbol) {
  case value_type('*'):
    data = replication_data(0, limits::max());
    break;
  case value_type('?'):
    data = replication_data(0, 1);
    break;
  case value_type('+'):
    data = replication_data(1, limits::max());
    break;
  }
  return !error();
}

// {m} {m,} {m,n}
template <typename InputIt, typename Traits>
bool 
replication_reader<InputIt,Traits>::read_braced_range(replication_data& data) {
  token tok;
  ts.get(tok);
  assert(tok.type == token_type::L_BRACE);

  std::size_t lower {0};
  std::size_t upper {0};
  // The range must begin with a number.
  if (!read_number(lower)) return false;

  if (!ts.get(tok)) return false;
  
  switch (tok.type) {
  case token_type::COUNT_SEP:
    if (!read_upper(upper)) return false;
    if (!verify_closing(lower, upper)) return false;
    break;
  case token_type::R_BRACE:
    upper = lower;
    break;
  default:
    set_error(error_type::error_badbrace);
  }

  data = replication_data(lower, upper);
  return !error();
}

template <typename InputIt, typename Traits>
bool replication_reader<InputIt,Traits>::read_upper(std::size_t& upper) {
  using limits = std::numeric_limits<std::size_t>;

  token tok;
  if (!ts.get(tok)) return false;
  if (tok.type == token_type::R_BRACE) {
    ts.putback(tok);
    upper = limits::max();
    return true;
  }
  if (tok.type != token_type::DIGIT) {
    set_error(error_type::error_badbrace);
    return false;
  }
  ts.putback(tok);

  return read_number(upper);
}        

template <typename InputIt, typename Traits>
bool replication_reader<InputIt,Traits>::read_number(std::size_t& num) {
  token tok;
  std::size_t val {0};
  for (int digit {0}; ts.get(tok) && tok.type == token_type::DIGIT; ) {
    digit = traits_.value(tok.ch, radix);
    val = radix * val + digit;
  }
  num = val;
  if (tok.type != token_type::DIGIT) ts.putback(tok);
  return true;
}

template <typename InputIt, typename Traits>
bool replication_reader<InputIt,Traits>::verify_closing(std::size_t lower, 
    std::size_t upper) {
  if (lower > upper) {
    set_error(error_type::error_badbrace);
    return false;
  }

  token tok;
  if (!ts.get(tok)) return false;
 
  if (tok.type != token_type::R_BRACE) {
    set_error(error_type::error_badbrace);
    return false;
  }
  return !error();
}

}//namespace lex
#endif// _replication_reader_h_
