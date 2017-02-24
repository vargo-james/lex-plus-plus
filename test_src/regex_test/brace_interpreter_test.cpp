#include "regex/brace_interpreter.h"
#include "interpreter_declarations.h"
#include "ttest/ttest.h"

template <typename CharT>
void verify_correct_limits(ttest::error_log& log, 
    const basic_string<CharT>& str, syntax_option_type f, size_t lower, 
    size_t upper) {
  Traits<CharT> traits;
  auto source = make_character_source(str, traits);
  error_type ec {error_type::error_none};
  
  auto interpreter = brace_interpreter<decltype(source)>(source, ec, f);

  auto range = interpreter.get();
  if (!range) {
    log.append("Failed get(): " + source_to_string(source));
  }

  if (range->first != lower) {
    log.append("error lower : " + std::to_string(range->first) + " != " +
        std::to_string(lower));
  }

  if (range->second != upper) {
    log.append("error upper : " + std::to_string(range->second) + " != " +
        std::to_string(upper));
  }
}

template <typename CharT>
void check_brace_error(ttest::error_log& log, const basic_string<CharT>& str,
  syntax_option_type f, regex_constants::error_type error) {
  Traits<CharT> traits;
  auto source = make_character_source(str, traits);
  error_type ec {error_type::error_none};
  
  auto interpreter = brace_interpreter<decltype(source)>(source, ec, f);

  auto range = interpreter.get();
  if (range) {
    log.append("invalid read: " + std::to_string(range->first) + " " + 
        std::to_string(range->second));
  }

  if (ec != error) {
    log.append("incorrect error");
  }
}

void basic_brace_test(ttest::error_log& log) {
  verify_correct_limits<char>(log, "4\\}", regex_constants::basic, 4, 4);
  verify_correct_limits<char>(log, "4,\\}", regex_constants::basic, 4, MAX);
  verify_correct_limits<char>(log, "4,7\\}", regex_constants::basic, 4, 7);
  verify_correct_limits<char>(log, "4,22\\}", regex_constants::basic, 4, 22);
  verify_correct_limits<char>(log, "14,223\\}", regex_constants::basic, 14, 
      223);

  using error_type = regex_constants::error_type;

  check_brace_error<char>(log, "4", regex_constants::basic, 
      error_type::error_brace);
  check_brace_error<char>(log, "4a", regex_constants::basic, 
      error_type::error_badbrace);
  check_brace_error<char>(log, "4}", regex_constants::basic, 
      error_type::error_badbrace);
  check_brace_error<char>(log, "4,3\\}", regex_constants::basic, 
      error_type::error_badbrace);
  check_brace_error<char>(log, "4,32", regex_constants::basic, 
      error_type::error_brace);
}

void extended_brace_test(ttest::error_log& log) {
  verify_correct_limits<char>(log, "4}", regex_constants::extended, 4, 4);
  verify_correct_limits<char>(log, "4,}", regex_constants::extended, 4, MAX);
  verify_correct_limits<char>(log, "4,7}", regex_constants::extended, 4, 7);
  verify_correct_limits<char>(log, "4,22}", regex_constants::extended, 4, 22);
  verify_correct_limits<char>(log, "14,223}", regex_constants::extended, 14, 
      223);

  check_brace_error<char>(log, "4", regex_constants::extended, 
      error_type::error_brace);
  check_brace_error<char>(log, "4a", regex_constants::extended, 
      error_type::error_badbrace);
  check_brace_error<char>(log, "4\\}", regex_constants::extended, 
      error_type::error_badbrace);
}

ttest::test_suite::pointer create_brace_interpreter_test() {
  using ttest::create_test;

  return create_test("brace interpreter", {
      create_test("basic", basic_brace_test),
      create_test("extended", extended_brace_test),
    });
}
