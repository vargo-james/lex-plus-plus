#include "bracket_test.h"
#include "regex/range.h"
#include "regex/regex_bracket.h"

//#include <iostream>
#include <string>
#include <vector>

using namespace lex;
using namespace detail;

template <typename PredT>
bool verify_pred(const PredT& p, const std::string& inputs, 
    const std::vector<bool>& outputs) {
  assert(inputs.size() == outputs.size());
  
  for (auto i = 0u; i != inputs.size(); ++i) {
    if (p(inputs[i]) != outputs[i]) return false;
  }

  return true;
}

void singleton_bracket_test(ttest::error_log& log) {
  using std::string;
  using Iter = typename string::const_iterator;

  string reg {"[abc]"};
  range<Iter> r(reg);
  
  bracket_reader<char> reader;
  auto pred = reader.read(r);

  bool b = 
    verify_pred(pred, "abdcef", {true, true, false, true, false, false});

  if (!b) {
    log.append("matching");
  }

  string not_reg {"[^abc]"};
  range<Iter> nr(not_reg);
  
  pred = reader.read(nr);

  b = verify_pred(pred, "abdcef", {false, false, true, false, true, true});

  if (!b) {
    log.append("non_matching");
  }
}

void close_dash_test(ttest::error_log& log) {
  using std::string;
  using Iter = typename string::const_iterator;

  string reg {"[]-a]"};
  range<Iter> r(reg);

  bracket_reader<char> reader;
  auto pred = reader.read(r);
  bool b = verify_pred(pred, "xya]-", {false, false, true, true, true});
  if (!b) {
    log.append("both, matching");
  }

  reg = {"[^]-]"};
  range<Iter> ran(reg);
  pred = reader.read(ran);

  b = verify_pred(pred, "xya]-", {true, true, true, false, false});
  if (!b) {
    log.append("both, non_matching");
  }
}

void range_test(ttest::error_log& log) {
  using std::string;
  using Iter = typename string::const_iterator;

  string reg {"[a-d]"};
  range<Iter> r(reg);

  bracket_reader<char> reader;
  auto pred = reader.read(r);
  bool b = verify_pred(pred, "aebg", {true, false, true, false});
  if (!b) {
    log.append("single matching");
  }

  reg = "[^a-d]";
  range<Iter> ran(reg);
  pred = reader.read(ran);
  b = verify_pred(pred, "aebg", {false, true, false, true});
  if (!b) {
    log.append("single non_matching");
  }

  reg = "[a-dX-Z]";
  range<Iter> ran2(reg);
  pred = reader.read(ran2);
  b = verify_pred(pred, "aebgY", {true, false, true, false, true});
  if (!b) {
    log.append("double matching");
  }

}
