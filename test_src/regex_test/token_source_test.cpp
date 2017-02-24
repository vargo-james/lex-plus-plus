#include "regex/token_source.h"
#include "interpreter_declarations.h"
#include "ttest/ttest.h"

template <typename CharT>
void verify_tokens(ttest::error_log& log, 
    const basic_string<CharT>& str, syntax_option_type syntax, 
    const vector<Token<CharT>>& expected) {
  Traits<CharT> traits;
  auto char_source = make_character_source(str, traits);
  error_type ec {error_type::error_none};
  
  token_source<decltype(char_source)> source(char_source, ec, syntax);

  for (const auto& token : expected) {
    auto match_p = source.get();
    if (!match_p) {
      log.append("Failed get(): " + source_to_string(char_source));
      break;
    }

    if (*match_p != token) {
      log.append("Incorrect get(): " + token_to_string(*match_p) + " != " + 
          token_to_string(token));
    }
  }

  log.append_if("still not empty: " + source_to_string(char_source), 
      !source.empty());

  auto match_p = source.get();
  if (match_p) {
    log.append("extra get(): " + token_to_string(*match_p));
  }
}

void token_source_test(ttest::error_log& log) {
  verify_tokens<wchar_t>(log, L"ab\\u0030\x5a", 
      syntax_option_type::ECMAScript, {
        Token<wchar_t>(L"ab0Z", token_type::STRING_LITERAL)
      });
  verify_tokens<char>(log, "\\.\\{12,\\}", syntax_option_type::basic, {
        Token<char>(".", token_type::STRING_LITERAL),
        Token<char>((size_t) 12, token_type::REP_LOWER),
        Token<char>(MAX, token_type::REP_UPPER),
      });

  verify_tokens<char>(log, "ab[^[:alnum:]]6", syntax_option_type::extended, {
        Token<char>("ab", token_type::STRING_LITERAL),
        Token<char>('[', token_type::L_BRACKET),
        Token<char>('^', token_type::NEGATION),
        Token<char>("alnum", token_type::CLASS),
        Token<char>(']', token_type::R_BRACKET),
        Token<char>("6", token_type::STRING_LITERAL),
      });

  verify_tokens<char>(log, "[ab][^bc]\\{2,3\\}", syntax_option_type::basic, {
        Token<char>('[', token_type::L_BRACKET),
        Token<char>('a', token_type::BRACKET_LITERAL),
        Token<char>('b', token_type::BRACKET_LITERAL),
        Token<char>(']', token_type::R_BRACKET),
        Token<char>('[', token_type::L_BRACKET),
        Token<char>('^', token_type::NEGATION),
        Token<char>('b', token_type::BRACKET_LITERAL),
        Token<char>('c', token_type::BRACKET_LITERAL),
        Token<char>(']', token_type::R_BRACKET),
        Token<char>((size_t) 2, token_type::REP_LOWER),
        Token<char>((size_t) 3, token_type::REP_UPPER),
      });

  verify_tokens<char>(log, ".(:=)(:a)", syntax_option_type::ECMAScript, {
        Token<char>('.', token_type::CHAR_CLASS),
        Token<char>('(', token_type::L_PAREN),
        Token<char>('=', token_type::ASSERTION),
        Token<char>(')', token_type::R_PAREN),
        Token<char>('(', token_type::L_PAREN),
        Token<char>(":a", token_type::STRING_LITERAL),
        Token<char>(')', token_type::R_PAREN),
      });
}

ttest::test_suite::pointer create_token_source_test() {
  using ttest::create_test;

  return create_test("token source", token_source_test);
}
