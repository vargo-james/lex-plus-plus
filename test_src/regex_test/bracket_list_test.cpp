#include "regex/bracket_list.h"
#include "bracket_list_test.h"

#include <cassert>
#include <cctype>
#include <functional>

using namespace lex;

using PredT = std::function<bool(char)>;

bool verify(const PredT& p, const std::string& inputs, 
    const std::vector<bool>& outputs) {
  assert(inputs.size() == outputs.size());
  
  for (auto i = 0u; i != inputs.size(); ++i) {
    if (p(inputs[i]) != outputs[i]) return false;
  }

  return true;
}

void matching_bracket_list_test(ttest::error_log& log) {
  PredT a = [](auto ch) {return std::isalnum(ch);};
  PredT b = [](auto ch) {return ch == '_';};
  PredT c = [](auto ch) {return ch == ';';};
  PredT d = [](auto ch) {return ch == '.';};

  bracket_list<PredT> list;
  list.add_element(a);
  list.add_element(b);
  list.add_element(std::move(c));
  list.add_element(std::move(d));

  auto combination = list.combine();

  if (!verify(combination, "a5t_;.^%#", {true, true, true, true, true, true,
        false, false, false})) {
    log.append("fail");
  }
}

void non_matching_bracket_list_test(ttest::error_log& log) {
  PredT a = [](auto ch) {return std::isdigit(ch);};
  PredT b = [](auto ch) {return ch == '_';};
  PredT c = [](auto ch) {return ch == ';';};
  PredT d = [](auto ch) {return ch == '.';};

  bracket_list<PredT> list;
  list.set_matching(false);
  list.add_element(a);
  list.add_element(b);
  list.add_element(std::move(c));
  list.add_element(std::move(d));

  auto combination = list.combine();

  if (!verify(combination, "Pqh5;^%#", {true, true, true, false, false, true,
        true, true})) {
    log.append("fail");
  }
}
