#ifndef _regex_test_declarations_h_
#define _regex_test_declarations_h_

#include "regex/character_source.h"
#include "regex/expression_token_stream.h"

#include <regex>
#include <string>

using namespace lex;

using std::basic_string;
using std::string;
using std::wstring;

using Iter = typename string::const_iterator;
using WideIter = typename wstring::const_iterator;

using Traits = std::regex_traits<char>;
using WideTraits = std::regex_traits<wchar_t>;

using Source = character_source<Iter, Traits>;
using WideSource = character_source<WideIter, WideTraits>;

using Stream = expression_token_stream<Source>;
using WideStream = expression_token_stream<WideSource>;

#endif// _regex_test_declarations_h_
