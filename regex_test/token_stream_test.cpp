#include "token_stream_test.h"
#include "regex/token_stream.h"

#include <regex>
#include <string>

using namespace lex;


template <typename InputIt, typename Traits>
bool compare_token_types(token_stream<InputIt,Traits>& ts, 
    const std::vector<token_type>& types) {
  using token = typename token_stream<InputIt,Traits>::token;
  token tok;
  for (auto t: types) {
    ts.get(tok);
    if (t != tok.type) {
      return false;
    }
  }
  return true;
}

void literals_test(ttest::error_log& log) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  using stream_type = token_stream<Iter,Traits>;

  regex_token<char,Traits> token;
  int count {0};

  const std::string reg0 {R"(abc\*\[)"};
  regex_range<Iter> ran0(reg0.begin(), reg0.end());
  regex_constants::error_type ec;
  stream_type ts0(ran0, regex_constants::extended, ec);
  while (ts0.get(token)) {
    ++count;
  }
  if (count != 5) {
    log.append("bad count");
  }


  const std::string reg {R"(a\*b&'>\^\[x\(yz)"};
  regex_range<Iter> ran(reg.begin(), reg.end());
  token_stream<Iter,Traits> ts(ran, regex_constants::extended, ec);


  count = 0;
  for (; ts.get(token); ++count) {
    if (token.type != token_type::LITERAL) {
      log.append("extended literal");
    }
  }
  if (count != 12) {
    log.append("extended count");
  }

  const std::string grep_reg {"\nab()+_{}\("};
  regex_range<Iter> grep_ran(grep_reg.begin(), grep_reg.end());
  token_stream<Iter,Traits> ts2(grep_ran, regex_constants::grep, ec);
  ts2.get(token);
  if (token.type != token_type::ALTERNATION) {
    log.append("grep alternation");
  }

  count = 0;
  for (;ts2.get(token); ++count) {
    if (token.type != token_type::LITERAL) {
      log.append("grep literal");
    }
  } 
  if (count != 9) {
    log.append("grep count");
  }

  const std::string ecma_reg {R"(]\[\]\a\p\q\v\b\u\*\31)"};
  regex_range<Iter> ecma_ran(ecma_reg.begin(), ecma_reg.end());
  token_stream<Iter,Traits> ts3(ecma_ran, regex_constants::ECMAScript, ec);
  bool escapes = compare_token_types(ts3, {
      token_type::LITERAL,
      token_type::LITERAL,
      token_type::LITERAL,
      token_type::LITERAL,
      token_type::LITERAL, 
      token_type::LITERAL, 
      token_type::LITERAL,
      token_type::ASSERTION,
      token_type::UNICODE_ESCAPE,
      token_type::LITERAL, 
      token_type::BACK_REF, 
      token_type::LITERAL, 
    });
  if (!escapes) {
    log.append("escapes");
  }
}


void replication_test(ttest::error_log& log) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  using stream_type = token_stream<Iter,Traits>;

  const std::string reg {R"(*?+)"};
  regex_range<Iter> ran(reg.begin(), reg.end());
  regex_constants::error_type ec;
  stream_type ts(ran, regex_constants::extended, ec);

  regex_token<char,Traits> token;
  while (ts.get(token)) {
    if (token.type != token_type::REPLICATION) {
      log.append("extended replications");
    }
  }

  const std::string reg2 {R"({3,66X})"};
  regex_range<Iter> ran2(reg2.begin(), reg2.end());
  stream_type ts2(ran2, regex_constants::extended, ec);
  if (!compare_token_types(ts2, {
        token_type::L_BRACE,
        token_type::DIGIT,
        token_type::COUNT_SEP,
        token_type::DIGIT,
        token_type::DIGIT,
        token_type::LITERAL,
        //token_type::R_BRACE
        })) {
    log.append("extended brace");
  }

  const std::string basic_reg {R"(\{2,})"};
  regex_range<Iter> basic_ran(basic_reg.begin(), basic_reg.end());
  stream_type ts3(basic_ran, regex_constants::basic, ec);
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

  const std::string reg0 {"[[::]]"};
  regex_range<Iter> ran0(reg0.begin(), reg0.end());
  regex_constants::error_type ec;
  stream_type ts0(ran0, regex_constants::extended, ec);
  if (!compare_token_types(ts0, {
        token_type::L_BRACKET,
        token_type::L_CLASS,
        //token_type::LITERAL,
        //token_type::LITERAL,
        //token_type::LITERAL,
        token_type::R_CLASS,
        //token_type::R_BRACKET
        })) {
    log.append("character class");
  }
  
  const std::string reg {R"([^ab)[:xy:]-[..]])"};
  regex_range<Iter> ran(reg.begin(), reg.end());
  stream_type ts(ran, regex_constants::extended, ec);
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
    log.append("extended bracket");
  }
}

void subexpr_test(ttest::error_log& log) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  using stream_type = token_stream<Iter,Traits>;
  const std::string reg0 {R"(a(?:)))"};
  regex_range<Iter> ran0(reg0.begin(), reg0.end());
  regex_constants::error_type ec;
  stream_type ts0(ran0, regex_constants::ECMAScript, ec);
  if (!compare_token_types(ts0, {
        token_type::LITERAL,
        token_type::L_PAREN,
        token_type::NO_SUBEXP,
        token_type::R_PAREN,
        token_type::R_PAREN,
        })) {
    log.append("ECMAScript no subexpr");
  }



  const std::string reg {R"(a(?:)\?)(?=)"};
  regex_range<Iter> ran(reg.begin(), reg.end());
  stream_type ts(ran, regex_constants::ECMAScript, ec);

  if (!compare_token_types(ts, {
        token_type::LITERAL,
        token_type::L_PAREN,
        token_type::NO_SUBEXP,
        token_type::R_PAREN,
        token_type::LITERAL,
        token_type::R_PAREN,
        token_type::L_PAREN,
        token_type::ASSERTION
        })) {
    log.append("ECMAScript subexpr");
  }

  regex_range<Iter> ran_again(reg.begin(), reg.end());
  stream_type ts2(ran_again, regex_constants::extended, ec);
  if (!compare_token_types(ts2, {
        token_type::LITERAL,
        token_type::L_PAREN,
        token_type::REPLICATION,
        token_type::LITERAL,
        token_type::R_PAREN,
        token_type::LITERAL,
        token_type::LITERAL,
        token_type::L_PAREN,
        token_type::REPLICATION,
        token_type::LITERAL
        })) {
    log.append("extended subexpr");
  }
}

void icase_test(ttest::error_log& log) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  using stream_type = token_stream<Iter,Traits>;
  const std::string reg {R"(a(b)"};
  regex_constants::error_type ec;
  regex_range<Iter> ran(reg.begin(), reg.end());
  stream_type ts(ran, regex_constants::basic | regex_constants::icase, ec);

  regex_token<char,Traits> token;
  if (!ts.get(token) || token.ch != 'A' || !ts.get(token) ||
      token.ch != '(' || !ts.get(token) || token.ch != 'B'
      || ts.get(token)) {
    log.append("icase");
  }
}

bool error_validation(const std::string& reg, 
    regex_constants::syntax_option_type f, int valid_tokens, 
    regex_constants::error_type et) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;
  regex_constants::error_type ec {regex_constants::error_type::error_none};
  
  regex_range<Iter> ran(reg.begin(), reg.end());
  token_stream<Iter,Traits> ts(ran, f, ec);
  int count {0};
  regex_token<char,Traits> token;
  while (ts.get(token)) {
    ++count;
  }
  return count == valid_tokens && ts.error() == et;
}


void stream_errors(ttest::error_log& log, 
    regex_constants::syntax_option_type f) {
  bool bad_escape = error_validation("abc\\d", f, 3, 
      regex_constants::error_escape);
  if (!bad_escape) {
    log.append("bad escape");
  }
  bad_escape = error_validation("abc\\", f, 3, regex_constants::error_escape);
  if (!bad_escape) {
    log.append("bad escape 2");
  }
  bool unmatched_paren = error_validation("abc\\)", f, 3, 
      regex_constants::error_paren);
  if (!unmatched_paren) {
    log.append("unmatched paren");
  }
  unmatched_paren = error_validation("abc\\([^]]\\)\\)", f, 9, 
      regex_constants::error_paren);
  if (!unmatched_paren) {
    log.append("unmatched paren 2");
  }
  bool bad_brace = error_validation("a\\{x\\}", f, 2, 
      regex_constants::error_badbrace);
  if (!bad_brace) {
    log.append("bad brace range");
  }
  bad_brace = error_validation("a\\{4,65\\}\\{1\\", f, 9, 
      regex_constants::error_badbrace);
  if (!bad_brace) {
    log.append("bad brace range 2");
  }

  bad_brace = error_validation("abc\\}",f,3,regex_constants::error_escape);
  if (!bad_brace) {
    log.append("unmatched right brace");
  }

}

void basic_stream_errors(ttest::error_log& log) {
  stream_errors(log, regex_constants::basic);
  stream_errors(log, regex_constants::grep);
}

void right_delimiter_test(ttest::error_log& log) {
  bool b = error_validation("(abcd", regex_constants::extended, 5, 
      regex_constants::error_paren);
  if (!b) {
    log.append("unmatched left paren");
  }
  b = error_validation("[[:::]", regex_constants::extended, 4, 
      regex_constants::error_brack);
  if (!b) {
    log.append("unmatched left bracket");
  }
  b = error_validation("[[:::", regex_constants::extended, 4, 
      regex_constants::error_brack);
  if (!b) {
    log.append("unmatched left bracket 2");
  }
  b = error_validation("xy{45,,,", regex_constants::extended, 8, 
      regex_constants::error_brace);
  if (!b) {
    log.append("unmatched left brace");
  }
}
