#ifndef _expression_interpreter_h_
#define _expression_interpreter_h_

#include "data_structures/optional.h"
#include "regex_token.h"
#include "regex_types.h"
#include "data_structures/simple_buffer.h"
#include "character_interpreter.h"
#include "brace_interpreter.h"

#include <cassert>
#include <cstddef>
#include <limits>

namespace lex {

template <typename Source> 
class expression_interpreter : public regex_constants {
 public:
  using char_type = typename Source::value_type;
  using traits_type = typename Source::traits_type;
  using value_type = regex_token<char_type, traits_type>;

  expression_interpreter(Source& src, error_type& ec, syntax_option_type f)
    : interpreter(src, ec, f, regex_constants::expression),
      brace_reader(src, ec, f) {}

  optional<value_type> get();
  bool empty() const {return buffer.empty() && interpreter.empty();}

 private:
  character_interpreter<Source> interpreter;
  brace_interpreter<Source> brace_reader;
  simple_buffer<value_type> buffer;

  optional<value_type> get_token();
};

template <typename CharT>
std::pair<std::size_t,std::size_t> get_limits(CharT ch) {
  using std::numeric_limits;
  using std::size_t;
  using std::pair;

  switch (ch) {
  case CharT('?'):
    return pair<size_t, size_t>(0, 1);
  case CharT('+'):
    return pair<size_t, size_t>(1, numeric_limits<size_t>::max());
  case CharT('*'):
    return pair<size_t, size_t>(0, numeric_limits<size_t>::max());
  default:
    assert(ch == CharT('*'));
  }
  return pair<size_t, size_t>(0, 0);
}

template <typename Source>
optional<typename expression_interpreter<Source>::value_type> 
expression_interpreter<Source>::get() {
  auto value = get_token();
  if (!value) {
    return value;
  }

  if (value->type == token_type::L_BRACE) {
    auto limits = brace_reader.get();
    if (!limits) return {};
    buffer.push(value_type(limits->second, token_type::REP_UPPER));
    return value_type(limits->first, token_type::REP_LOWER);
  }

  if (value->type == token_type::REPLICATION) {
    auto limits = get_limits(value->ch);
    buffer.push(value_type(limits.second, token_type::REP_UPPER));
    return value_type(limits.first, token_type::REP_LOWER);
  }

  if (value->type != token_type::LITERAL) {
    return value;
  }

  typename traits_type::string_type string_literal;
  for (; value && value->type == token_type::LITERAL; value = get_token()) {
    string_literal.push_back(value->ch);
  }

  if (value) {
    buffer.push(*value);
  }

  return value_type(string_literal, token_type::STRING_LITERAL);
}

template <typename Source>
optional<typename expression_interpreter<Source>::value_type> 
expression_interpreter<Source>::get_token() {
  if (!buffer.empty()) {
    return buffer.get();
  }
  return interpreter.get();
}

}//namespace lex
#endif// _expression_interpreter_h_
