#ifndef _interpreter_declarations_h_
#define _interpreter_declarations_h_

#include "regex_types.h"
#include "regex/character_source.h"
#include "regex/table_interpreter.h"
#include "ttest/ttest.h"

#include <regex>
#include <string>
#include <vector>

using namespace lex;

using std::basic_string;
using std::string;
using std::wstring;
using std::vector;
using std::numeric_limits;
using std::size_t;

static size_t MAX = numeric_limits<size_t>::max();

template <typename CharT>
using Traits = std::regex_traits<CharT>;

template <typename CharT>
using Token = regex_token<CharT, Traits<CharT>>;

using error_type = regex_constants::error_type;
using syntax_option_type = regex_constants::syntax_option_type;

template <typename CharT>
char to_char(CharT ch) {
  ch = (ch > 127)? '@' : ch;
  return static_cast<char>(ch);
}

template <typename String>
string to_narrow_string(const String& str) {
  string out;
  for (auto ch : str) {
    out.push_back(to_char(ch));
  }
  return out;
}

template <typename CharSource>
string source_to_string(CharSource& source) {
  string out;
  for (auto ch_p = source.get(); ch_p; ch_p = source.get()) {
    out.push_back(to_char(*ch_p));
  }
  return out;
}

template <typename CharT, typename Traits>
string token_to_string(const regex_token<CharT, Traits>& token) {
  string out;
  if (token.ch) {
    return string(1, to_char(token.ch));
  } 
  
  if (token.count) {
    return std::to_string(token.count);
  }

  if (!token.str.empty()) {
    Traits t{};
    auto source = make_character_source(token.str, t);
    return source_to_string(source);
  }
  return {};
}

#endif// _interpreter_declarations_h_
