#ifndef _character_interpreter_h_
#define _character_interpreter_h_

#include "character_markup_tables.h"
#include "error_tracker.h"
#include "markup_table.h"
#include "data_structures/optional.h"
#include "regex_token.h"
#include "regex_types.h"

#include <limits>

namespace lex {

template <typename Source>
class character_interpreter : public regex_constants {
 public:
  using char_type = typename Source::value_type;
  using traits_type = typename Source::traits_type;
  using value_type = regex_token<char_type,traits_type>;

  character_interpreter(Source& src, error_type& ec, syntax_option_type syntax, 
      context_type con);

  optional<value_type> get();
  bool empty() const {return source_.empty();}
    
 private:
  using table_type = markup_table<char_type, token_type>;
  using pretoken_type = marked_value<char_type, token_type>;
  using string_type = typename traits_type::string_type;

  Source& source_;
  table_type specials;
  table_type escapes;
  error_tracker tracker;
  char_type escape_key {'\\'};

  optional<pretoken_type> get_pretoken();

  // These read literals
  optional<value_type> get_unicode();
  value_type control_character(char_type ch) const;
};

template <typename CharT>
bool is_ASCII_letter(CharT ch) {
  bool is_upper = 64 < ch && ch < 91;
  bool is_lower = 96 < ch && ch < 123;
  return is_upper || is_lower;
}

template <typename Source>
character_interpreter<Source>::character_interpreter(Source& src, error_type& ec,
    syntax_option_type syntax, context_type con)
  : source_(src),
    specials(specials_table<char_type,traits_type>(con, syntax, 
          src.get_traits())),
    escapes(escapes_table<char_type,traits_type>(con, syntax, 
          src.get_traits())),
    tracker(ec) {
  switch (con) {
  case regex_constants::bracket:
    escape_key = char_type('[');
    break;
  case regex_constants::subexpression:
    escape_key = char_type(':');
    break;
  default:
    break;
  }
}

template <typename Source>
optional<typename character_interpreter<Source>::pretoken_type> 
character_interpreter<Source>::get_pretoken() {
  if (tracker.error() || source_.empty()) {
    return {};
  }

  auto first = source_.get();
  if (*first == escape_key && !source_.empty()) {
    auto escaped_char = source_.get();

    auto type = escapes.lookup(*escaped_char);
    if (type) {
      return pretoken_type{*escaped_char, *type};
    } else {
      source_.unget();
    }
  } 

  auto type = specials.lookup(*first);
  if (type) {
    return pretoken_type{*first, *type};
  } else {
    source_.unget();
  }
  return {};
}    

template <typename Source>
optional<typename character_interpreter<Source>::value_type> 
character_interpreter<Source>::get() {
  auto pretoken = get_pretoken();
  if (!pretoken) return {};

  switch (pretoken->type) {
  case token_type::SIMPLE_BACK_REF: {
    const int radix {10};
    std::size_t count {0};
    count = source_.get_traits().value(pretoken->value, radix);
    return value_type(count, token_type::BACK_REF);
  }
  case token_type::COMPOUND_BACK_REF: {
    source_.unget();
    auto count = source_.get_int();
    return value_type(*count, token_type::BACK_REF);
  }
  case token_type::HEX_ESCAPE: {
    const int digit_count {2};
    const int radix {16};
    auto count = source_.get_n_digit_int(digit_count, radix);
    if (!count) {
      return value_type(pretoken->value, token_type::LITERAL);
    }
    return value_type(char_type(*count), token_type::LITERAL);
  }
  case token_type::UNICODE_ESCAPE:
    return get_unicode();
  case token_type::CONTROL_CHAR:
    return control_character(pretoken->value);
  case token_type::CONTROL: {
    auto ch_p = source_.get();
    if (!ch_p || !is_ASCII_letter(*ch_p)) {
      tracker.set_error(error_type::error_escape);
      return {};
    }
    return value_type(char_type(*ch_p % 32), token_type::CONTROL);
  }
  case token_type::COUNT: {
    source_.unget();
    auto count = source_.get_int();
    return value_type(*count, token_type::COUNT);
  }
  case token_type::CLASS:
  case token_type::EQUIV:
  case token_type::COLLATE: {
    string_type limit(1, pretoken->value);
    limit.push_back(char_type(']'));
    auto string_val = source_.get_until(limit);
    return value_type(string_val, pretoken->type);
  }
  default:
    return value_type(pretoken->value, pretoken->type);
    break;
  }
}

template <typename Source>
optional<typename character_interpreter<Source>::value_type>
character_interpreter<Source>::get_unicode() {
  const int digit_count {4};
  const int radix {16};
  const char_type character {'u'};
  auto code = source_.get_n_digit_int(digit_count, radix);
  if (!code) {
    return value_type(character, token_type::CHAR_CLASS);
  }
  if (std::numeric_limits<char_type>::max() < code) {
    tracker.set_error(error_type::error_escape);
    return {};
  }
  return value_type(char_type(*code), token_type::LITERAL);
}

template <typename Source>
typename character_interpreter<Source>::value_type
character_interpreter<Source>::control_character(char_type ch) const {
  switch (ch) {
  case 'f':
    ch = '\f';
    break;
  case 'n':
    ch = '\n';
    break;
  case 'v':
    ch = '\v';
    break;
  case 'r':
    ch = '\r';
    break;
  case 't':
    ch = '\t';
    break;
  case '0':
    ch = '\0';
    break;
  }
  return value_type(ch, token_type::LITERAL);
}

}//namespace lex
#endif// _character_interpreter_h_
