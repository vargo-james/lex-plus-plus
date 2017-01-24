#include "token_stream_test.h"
#include "regex/token_stream.h"

#include <regex>
#include <string>

using namespace lex;


void literals_test(ttest::error_log& log) {

  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;

  const std::string reg {R"(a\*b&'>\n\[x\(yz)"};

  token_stream<Iter,Traits> ts(reg.begin(), reg.end(),
      std::regex_constants::extended);

  regex_token<char,Traits> token;
  int count(0);
  for (; ts.get(token); ++count) {
    if (token.type != token_type::LITERAL) {
      log.append("extended literal");
    }
  }
  if (count != 12) {
    log.append("extended count");
  }

  const std::string grep_reg {"ab()+_{}\\"};
  token_stream<Iter,Traits> ts2(grep_reg.begin(), grep_reg.end(),
      std::regex_constants::grep);
  count = 0;
  for (;ts2.get(token); ++count) {
    if (token.type != token_type::LITERAL) {
      log.append("grep literal");
    }
  } 
  if (count != 9) {
    log.append("grep count");
  }
}


template <typename InputIt, typename Traits>
bool compare_token_types(token_stream<InputIt,Traits>& ts, 
    const std::vector<token_type>& types) {
  using token_type = typename token_stream<InputIt,Traits>::token;
  token_type tok;
  for (auto t: types) {
    ts.get(tok);
    if (t != tok.type) {
      return false;
    }
  }
  return true;
}

void replication_test(ttest::error_log& log) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  using stream_type = token_stream<Iter,Traits>;

  const std::string reg {R"(*?+)"};
  stream_type ts(reg.begin(), reg.end(), std::regex_constants::extended);

  regex_token<char,Traits> token;
  while (ts.get(token)) {
    if (token.type != token_type::REPLICATION) {
      log.append("extended replications");
    }
  }

  const std::string reg2 {R"({3,66X})"};
  stream_type ts2(reg2.begin(), reg2.end(), std::regex_constants::extended);
  if (!compare_token_types(ts2, {
        token_type::L_BRACE,
        token_type::DIGIT,
        token_type::COUNT_SEP,
        token_type::DIGIT,
        token_type::DIGIT,
        token_type::LITERAL,
        token_type::R_BRACE
        })) {
    log.append("extended brace");
  }

  const std::string basic_reg {R"(\{2,})"};
  stream_type ts3(basic_reg.begin(), basic_reg.end(), 
      std::regex_constants::basic);
  if (!compare_token_types(ts3, {
        token_type::L_BRACE,
        token_type::DIGIT,
        token_type::COUNT_SEP,
        token_type::LITERAL
        })) {
    log.append("basic brace");
  }
}

void bracket_test(ttest::error_log& log) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  using stream_type = token_stream<Iter,Traits>;
  const std::string reg {R"([^ab)[:xy:]-[..]])"};
  stream_type ts(reg.begin(), reg.end(), std::regex_constants::extended);

  if (!compare_token_types(ts, {
        token_type::L_BRACKET,
        token_type::NEGATION,
        token_type::LITERAL,
        token_type::LITERAL,
        token_type::LITERAL,
        token_type::L_CLASS,
        token_type::LITERAL,
        token_type::LITERAL,
        token_type::R_CLASS,
        token_type::RANGE_DASH,
        token_type::L_COLLATE,
        token_type::R_COLLATE,
        token_type::R_BRACKET
        })) {
    log.append("bracket");
  }
}

void subexpr_test(ttest::error_log& log) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  using stream_type = token_stream<Iter,Traits>;
  const std::string reg {R"(a(?:)(\?)(?=)"};
  stream_type ts(reg.begin(), reg.end(), std::regex_constants::ECMAScript);


  if (!compare_token_types(ts, {
        token_type::LITERAL,
        token_type::L_PAREN,
        token_type::NO_SUBEXP,
        token_type::R_PAREN,
        token_type::L_PAREN,
        token_type::LITERAL,
        token_type::R_PAREN,
        token_type::L_PAREN,
        token_type::POS_LOOKAHEAD
        })) {
    log.append("subexpr");
  }
}

void icase_test(ttest::error_log& log) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  using stream_type = token_stream<Iter,Traits>;
  const std::string reg {R"(a(b)"};
  stream_type ts(reg.begin(), reg.end(), 
      std::regex_constants::basic | std::regex_constants::icase);

  regex_token<char,Traits> token;
  if (!ts.get(token) || token.value != 'A' || !ts.get(token) ||
      token.value != '(' || !ts.get(token) || token.value != 'B'
      || ts.get(token)) {
    log.append("icase");
  }
}
