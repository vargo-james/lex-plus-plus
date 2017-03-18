#ifndef _brace_interpreter_h_
#define _brace_interpreter_h_

#include "error_tracker.h"
#include "data_structures/optional.h"
#include "character_interpreter.h"

#include "project_assert.h"
#include <cstddef>
#include <utility>

namespace lex {

template <typename Source>
class brace_interpreter : public regex_constants {
 public:
  using size_type = std::size_t;
  using pair_type = std::pair<size_type, size_type>;

  brace_interpreter(Source& src, error_type& ec, syntax_option_type syntax)
    : interpreter(src, ec, syntax, regex_constants::brace),
      tracker(ec) {}

  optional<pair_type> get();

 private:
  character_interpreter<Source> interpreter;
  error_tracker tracker;

  bool check_if_empty();
  optional<size_type> get_lower();
  optional<size_type> get_after_comma();
};

template <typename Source>
bool brace_interpreter<Source>::check_if_empty() {
  if (interpreter.empty()) {
    tracker.set_error(error_type::error_brace);
    return true;
  }
  return false;
}

template <typename Source>
optional<typename brace_interpreter<Source>::pair_type>
brace_interpreter<Source>::get() {
  auto lower_p = get_lower();
  if (!lower_p) {
    return {};
  }

  if (check_if_empty()) return {};
  auto comma_or_close = interpreter.get();
  assert(tracker.no_error());
  assert(comma_or_close);
  switch (comma_or_close->type) {
  case token_type::R_BRACE:
    return pair_type(*lower_p, *lower_p);
  case token_type::COMMA: {
    auto upper_p = get_after_comma();
    if (!upper_p) return {};
    if (*lower_p > *upper_p) {
      tracker.set_error(error_type::error_badbrace);
      return {};
    }
    return pair_type(*lower_p, *upper_p);
  }
  default:
    tracker.set_error(error_type::error_badbrace);
    return {};
  }
  return {};
}

template <typename Source>
optional<typename brace_interpreter<Source>::size_type>
brace_interpreter<Source>::get_lower() {
  if (check_if_empty()) return {};

  auto lower_token = interpreter.get();
  assert(lower_token && tracker.no_error());

  if (lower_token->type != token_type::COUNT) {
    tracker.set_error(error_type::error_badbrace);
    return {};
  }

  return lower_token->count;
}

template <typename Source>
optional<typename brace_interpreter<Source>::size_type>
brace_interpreter<Source>::get_after_comma() {
  if (check_if_empty()) return {};
  size_type upper(0);

  auto upper_or_close = interpreter.get();
  assert(upper_or_close && tracker.no_error());

  switch (upper_or_close->type) {
  case token_type::R_BRACE:
    return std::numeric_limits<size_type>::max(); 
  case token_type::COUNT:
    upper = upper_or_close->count;
    break;
  default:
    tracker.set_error(error_type::error_badbrace);
    return {};
  }
  
  if (check_if_empty()) return {};
  auto close = interpreter.get();
  if (close->type != token_type::R_BRACE) {
    tracker.set_error(error_type::error_badbrace);
    return {};
  }

  return upper;
}

}//namespace lex
#endif// _brace_interpreter_h_
