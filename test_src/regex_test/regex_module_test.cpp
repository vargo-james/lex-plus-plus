#include "ttest/ttest.h"
#include "bracket_list_test.h"
#include "bracket_test.h"

ttest::test_suite::pointer create_character_source_test(); 
ttest::test_suite::pointer create_markup_table_test();
ttest::test_suite::pointer create_brace_interpreter_test();
ttest::test_suite::pointer create_expression_interpreter_test();
ttest::test_suite::pointer create_bracket_interpreter_test();
ttest::test_suite::pointer create_token_source_test();
ttest::test_suite::pointer create_compiler_impl_test();
ttest::test_suite::pointer create_compiler_test();
ttest::test_suite::pointer create_regex_test();

ttest::test_suite::pointer create_bracket_list_test();
ttest::test_suite::pointer create_bracket_test(); 

ttest::test_suite::pointer create_regex_module_test() {
  using namespace ttest;
  return create_test("regex module", {
      create_markup_table_test(),
      create_brace_interpreter_test(),
      create_bracket_interpreter_test(),
      create_expression_interpreter_test(),
      create_character_source_test(),
      create_token_source_test(),
      create_compiler_impl_test(),
      create_compiler_test(),
      create_bracket_list_test(),
      //create_bracket_test(), 
      create_regex_test(),
  });
}
