#include "replication_reader_test.h"
#include "regex/replication_reader.h"
#include "regex_types.h"

#include <cassert>
#include <string>

using namespace lex;

bool test_reader(const std::string& reg, 
    regex_constants::syntax_option_type f, size_t expected_lower, 
    size_t expected_upper) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;

  regex_constants::error_type ec {regex_constants::error_none};
  regex_range<Iter> ran(reg.begin(), reg.end());
  token_stream<Iter,Traits> ts(ran, f, ec);

  Traits traits;
  replication_reader<Iter,Traits> reader(traits, ts, ec);

  replication_data data(15,15);

  bool good_read = reader.read(data);

  return good_read && 
    expected_lower == data.lower &&
    expected_upper == data.upper &&
    ts.empty() &&
    !ts.error();
}

void replication_symbol_test(ttest::error_log& log) {
  using limits = std::numeric_limits<std::size_t>;

  bool first = test_reader("*", regex_constants::basic, 0, limits::max());
  if (!first) {
    log.append("b*");
  }
  first = test_reader("+", regex_constants::extended, 1, limits::max());
  if (!first) {
    log.append("e+");
  }
  first = test_reader("\\+", regex_constants::basic, 1, limits::max());
  if (!first) {
    log.append("b+");
  }
  first = test_reader("?", regex_constants::ECMAScript, 0, 1);
  if (!first) {
    log.append("EC+");
  }
  first = test_reader("\\?", regex_constants::basic, 0, 1);
  if (!first) {
    log.append("b+");
  }
}

void replication_reader_test(ttest::error_log& log) {
  using limits = std::numeric_limits<std::size_t>;

  bool first = test_reader("{5}", regex_constants::extended, 5, 5);
  if (!first) {
    log.append("single digit extended");
  }
  first = test_reader("\\{5\\}", regex_constants::basic, 5, 5);
  if (!first) {
    log.append("single digit basic");
  }
  first = test_reader("{125}", regex_constants::extended, 125, 125);
  if (!first) {
    log.append("multi digit extended");
  }
  first = test_reader("{12,}", regex_constants::extended, 12, limits::max());
  if (!first) {
    log.append("lower no upper extended");
  }
  first = test_reader("{12,22}", regex_constants::extended, 12, 22);
  if (!first) {
    log.append("lower,upper extended");
  }
}

bool error_checker(const std::string& reg, 
    regex_constants::error_type expected_error) {
  using Iter = typename std::string::const_iterator;
  using Traits = std::regex_traits<char>;

  regex_constants::error_type ec {regex_constants::error_none};
  regex_range<Iter> ran(reg.begin(), reg.end());
  token_stream<Iter,Traits> ts(ran, regex_constants::egrep,
      ec);
  Traits traits;
  replication_reader<Iter,Traits> reader(traits, ts, ec);

  replication_data data(15,15);

  return !reader.read(data) && ts.error() == expected_error;
}

void replication_errors_test(ttest::error_log& log) {
  bool b = error_checker("{x5}", regex_constants::error_badbrace);
  if (!b) {
    log.append("initial x");
  }
}

