#include "interpreter_declarations.h"
#include "regex/expression_interpreter.h"
#include "ttest/ttest.h"

template <typename CharT>
void verify_correct_tokens(ttest::error_log& log, 
    const basic_string<CharT>& str, syntax_option_type f, 
    const vector<Token<CharT>>& expected) {
  Traits<CharT> traits;
  auto source = make_character_source(str, traits);
  error_type ec {error_type::error_none};
  
  auto interpreter = expression_interpreter<decltype(source)>(source, ec, f);

  for (const auto& token : expected) {
    auto match_p = interpreter.get();
    if (!match_p) {
      log.append("Failed get(): " + source_to_string(source));
      break;
    }

    if (*match_p != token) {
      log.append("Incorrect get(): " + token_to_string(*match_p) + " != " + 
          token_to_string(token));
    }
  }
  auto match_p = interpreter.get();
  if (match_p) {
    log.append("extra get(): " + token_to_string(*match_p));
  }
}

void char_interpreter_literals(ttest::error_log& log) {
  verify_correct_tokens<char>(log, "abcd", syntax_option_type::basic, {
        Token<char>("abcd", token_type::STRING_LITERAL)
      });

  verify_correct_tokens<char>(log, "x+y=z", syntax_option_type::basic, {
        Token<char>("x+y=z", token_type::STRING_LITERAL)
      });
  verify_correct_tokens<char>(log, "x\\+y=z", syntax_option_type::extended, {
        Token<char>("x+y=z", token_type::STRING_LITERAL)
      });
  verify_correct_tokens<char>(log, "x\\+\\[]y=z", 
      syntax_option_type::extended, {
        Token<char>("x+[]y=z", token_type::STRING_LITERAL)
      });
  verify_correct_tokens<wchar_t>(log, L"\\u0030", 
      syntax_option_type::ECMAScript, {
        Token<wchar_t>(L"0", token_type::STRING_LITERAL)
      });
  verify_correct_tokens<wchar_t>(log, L"\\u0030\x5A", 
      syntax_option_type::ECMAScript, {
        Token<wchar_t>(L"0Z", token_type::STRING_LITERAL)
      });
  verify_correct_tokens<wchar_t>(log, L"ab\\u0030\x5a", 
      syntax_option_type::ECMAScript, {
        Token<wchar_t>(L"ab0Z", token_type::STRING_LITERAL)
      });
  verify_correct_tokens<char>(log, "ab\\n", syntax_option_type::ECMAScript, {
        Token<char>("ab\n", token_type::STRING_LITERAL)
      });
}

void char_interpreter_basic(ttest::error_log& log) {
  verify_correct_tokens<char>(log, "a+\\..\\[*", syntax_option_type::basic, {
        Token<char>("a+.", token_type::STRING_LITERAL),
        Token<char>('.', token_type::CHAR_CLASS),
        Token<char>("[", token_type::STRING_LITERAL),
        Token<char>((size_t) 0, token_type::REP_LOWER),
        Token<char>(MAX, token_type::REP_UPPER),
      });
  verify_correct_tokens<char>(log, "\\21a", syntax_option_type::basic, {
        Token<char>((size_t) 2, token_type::BACK_REF),
        Token<char>("1a", token_type::STRING_LITERAL),
      });
  verify_correct_tokens<char>(log, "\\x\\", syntax_option_type::basic, {
        Token<char>('x', token_type::REDUNDANT_ESCAPE),
        Token<char>('\\', token_type::TRAILING_ESCAPE),
      });
  verify_correct_tokens<char>(log, "\\(\n\\)", syntax_option_type::grep, {
        Token<char>('(', token_type::L_PAREN),
        Token<char>('\n', token_type::ALTERNATION),
        Token<char>(')', token_type::R_PAREN),
      });
  verify_correct_tokens<char>(log, ".\\{12,\\}", syntax_option_type::basic, {
        Token<char>('.', token_type::CHAR_CLASS),
        Token<char>((size_t) 12, token_type::REP_LOWER),
        Token<char>(MAX, token_type::REP_UPPER),
        });
  verify_correct_tokens<char>(log, "\\.\\{12,\\}", syntax_option_type::basic, {
        Token<char>(".", token_type::STRING_LITERAL),
        Token<char>((size_t) 12, token_type::REP_LOWER),
        Token<char>(MAX, token_type::REP_UPPER),
        });
}

void char_interpreter_extended(ttest::error_log& log) {
  verify_correct_tokens<char>(log, "\\21a", syntax_option_type::extended, {
        Token<char>((size_t) 2, token_type::BACK_REF),
        Token<char>("1a", token_type::STRING_LITERAL),
      });
  verify_correct_tokens<char>(log, "{1,2}", syntax_option_type::extended, {
        Token<char>((size_t) 1, token_type::REP_LOWER),
        Token<char>((size_t) 2, token_type::REP_UPPER),
        });
  verify_correct_tokens<char>(log, "ab{1}", syntax_option_type::extended, {
        Token<char>("ab", token_type::STRING_LITERAL),
        Token<char>((size_t) 1, token_type::REP_LOWER),
        Token<char>((size_t) 1, token_type::REP_UPPER),
        });
  verify_correct_tokens<char>(log, ".{12,}", syntax_option_type::extended, {
        Token<char>('.', token_type::CHAR_CLASS),
        Token<char>((size_t) 12, token_type::REP_LOWER),
        Token<char>(MAX, token_type::REP_UPPER),
        });
  verify_correct_tokens<char>(log, "(ab\\[)+", syntax_option_type::extended, {
        Token<char>('(', token_type::L_PAREN),
        Token<char>("ab[", token_type::STRING_LITERAL),
        Token<char>(')', token_type::R_PAREN),
        Token<char>((size_t) 1, token_type::REP_LOWER),
        Token<char>(MAX, token_type::REP_UPPER),
      });
  verify_correct_tokens<char>(log, "\n\\", syntax_option_type::egrep, {
        Token<char>('\n', token_type::ALTERNATION),
        Token<char>('\\', token_type::TRAILING_ESCAPE),
      });
}

void char_interpreter_ECMAScript(ttest::error_log& log) {
  verify_correct_tokens<char>(log, "\\21a", syntax_option_type::ECMAScript, {
        Token<char>((size_t) 21, token_type::BACK_REF),
        Token<char>("a", token_type::STRING_LITERAL),
      });
  verify_correct_tokens<char>(log, "\\u1a", syntax_option_type::ECMAScript, {
        Token<char>('u', token_type::CHAR_CLASS),
        Token<char>("1a", token_type::STRING_LITERAL),
      });
  verify_correct_tokens<char>(log, "\\u3a\\x3a", syntax_option_type::ECMAScript, {
        Token<char>('u', token_type::CHAR_CLASS),
        Token<char>("3a:", token_type::STRING_LITERAL),
      });
  verify_correct_tokens<char>(log, "3a\\x3", syntax_option_type::ECMAScript, {
        Token<char>("3ax3", token_type::STRING_LITERAL),
      });
  verify_correct_tokens<char>(log, "\\ca", syntax_option_type::ECMAScript, {
        Token<char>((char) ('a' % 32), token_type::CONTROL),
      });
  verify_correct_tokens<char>(log, "\\c1", syntax_option_type::ECMAScript, {
      });
}

ttest::test_suite::pointer create_expression_interpreter_test() {
  using namespace ttest;
  return create_test("expression_interpreter", {
      create_test("literals", char_interpreter_literals),
      create_test("basic", char_interpreter_basic),
      create_test("extended", char_interpreter_extended),
      create_test("ECMAScript", char_interpreter_ECMAScript),
      });
}
