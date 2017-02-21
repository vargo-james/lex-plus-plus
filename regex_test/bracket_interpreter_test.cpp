#include "regex/bracket_interpreter.h"
#include "interpreter_declarations.h"
#include "ttest/ttest.h"

template <typename CharT>
void verify_correct_bracket_tokens(ttest::error_log& log, 
    const basic_string<CharT>& str, syntax_option_type f, 
    const vector<Token<CharT>>& expected) {
  Traits<CharT> traits;
  auto source = make_character_source(str, traits);
  error_type ec {error_type::error_none};
  
  auto interpreter = bracket_interpreter<decltype(source)>(source, ec, f);

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

  log.append_if("NOT REAL. THIS JUST SILENCES AN INCONVENIENT WARNING FOR THIS"
      "PARTICULAR SOURCE FILE", MAX == 1);
}

void bracket_interpreter_test(ttest::error_log& log) {
  verify_correct_bracket_tokens<char>(log, "[1", syntax_option_type::basic, {
        Token<char>('[', token_type::BRACKET_LITERAL),
        Token<char>('1', token_type::BRACKET_LITERAL),
      });
  verify_correct_bracket_tokens<char>(log, "^]]", syntax_option_type::basic, {
        Token<char>('^', token_type::NEGATION),
        Token<char>(']', token_type::BRACKET_LITERAL),
        Token<char>(']', token_type::R_BRACKET),
      });

  verify_correct_bracket_tokens<char>(log, "[:alpha:]", 
      syntax_option_type::basic, {
        Token<char>("alpha", token_type::CLASS),
      });

  verify_correct_bracket_tokens<char>(log, "^[=alpha=]]", 
      syntax_option_type::basic, {
        Token<char>('^', token_type::NEGATION),
        Token<char>("alpha", token_type::EQUIV),
        Token<char>(']', token_type::R_BRACKET),
      });

  verify_correct_bracket_tokens<char>(log, "[.tilde.]]^]", 
      syntax_option_type::basic, {
        Token<char>("tilde", token_type::COLLATE),
        Token<char>(']', token_type::R_BRACKET),
        Token<char>('^', token_type::NEGATION),
        Token<char>(']', token_type::BRACKET_LITERAL),
      });

  verify_correct_bracket_tokens<char>(log, "---]", 
      syntax_option_type::basic, {
        Token<char>('-', token_type::BRACKET_LITERAL),
        Token<char>('-', token_type::RANGE_DASH),
        Token<char>('-', token_type::BRACKET_LITERAL),
        Token<char>(']', token_type::R_BRACKET),
      });

  verify_correct_bracket_tokens<char>(log, "^]---]", 
      syntax_option_type::basic, {
        Token<char>('^', token_type::NEGATION),
        Token<char>(']', token_type::BRACKET_LITERAL),
        Token<char>('-', token_type::RANGE_DASH),
        Token<char>('-', token_type::RANGE_DASH),
        Token<char>('-', token_type::BRACKET_LITERAL),
        Token<char>(']', token_type::R_BRACKET),
      });
}

ttest::test_suite::pointer create_bracket_interpreter_test() {
  using ttest::create_test;

  return create_test("bracket_interpreter", bracket_interpreter_test);
}
