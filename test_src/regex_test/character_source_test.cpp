#include "regex/character_source.h"
#include "ttest/ttest.h"

#include <regex>
#include <string>
#include <sstream>

using namespace lex;

using std::string;
using Traits = std::regex_traits<char>;
static Traits t;

void get_int_test(ttest::error_log& log, const string& str, int radix = 10) {
  size_t stoi_count {0};
  auto stoi_val = static_cast<unsigned>(stoi(str, &stoi_count, radix));

  auto src = make_character_source(str,t);

  auto reader_val = src.get_int(radix);

  if (!reader_val) {
    log.append("unable to get_int " + str);
  }
  if (stoi_val != *reader_val) {
    log.append("mismatching value: " + str);
  }
}

static int exp(int rad, int exp) {
  int val = 1;
  while (exp-- > 0) {
    val *= rad;
  }
  return val;
}

void get_n_digit_int_test(ttest::error_log& log, const string& str,
    size_t n, int radix = 10) {
  size_t stoi_count {0};
  auto stoi_val = static_cast<unsigned>(stoi(str, &stoi_count, radix));

  auto src = make_character_source(str,t);
  auto reader_val = src.get_n_digit_int(n, radix);

  if (stoi_count >= n) {
    if (!reader_val) {
      log.append("unable to get_n_digit_int " + str);
    }

    if (*reader_val != stoi_val/exp(radix, stoi_count - n)) {
      log.append("value mismatch " + str);
    }
  }
  if (stoi_count < n) {
    if (reader_val) {
      log.append("invalid read " + str);
    }
  }
}

void integer_getting_test(ttest::error_log& log) {
  get_int_test(log, "1234");
  get_int_test(log, "1234abc");
  get_int_test(log, "1234ABC");
  get_int_test(log, "1234abc, 16");

  get_n_digit_int_test(log, "12345", 3);
  get_n_digit_int_test(log, "12368", 7);
  get_n_digit_int_test(log, "123xy", 4);
  get_n_digit_int_test(log, "123fe", 3, 16);
  get_n_digit_int_test(log, "123fe", 5, 16);
}

void get_until_pos_test(ttest::error_log& log, const std::string& pre, 
    const std::string& limit, const std::string& post) {
  std::string reg = pre + limit + post;
  auto src = make_character_source(reg, t);

  auto result = src.get_until(limit);
  if (!std::equal(pre.begin(), pre.end(), result.begin(), result.end())) {
    log.append("get_until fail: " + reg + ": limit: " + limit);
  }
}

void get_until_neg_test(ttest::error_log& log, const std::string& reg, 
    const std::string& limit) {
  auto src = make_character_source(reg, t);

  auto result = src.get_until(limit);
  if (!std::equal(result.begin(), result.end(), reg.begin(), reg.end())) {
    log.append("get_until fail: " + reg + ": limit: " + limit);
  }
}

void get_until_test(ttest::error_log& log) {
  get_until_pos_test(log, "xhaach", "*(a", "zh1234");
  get_until_pos_test(log, "", "*(a", "zh1234");
  get_until_pos_test(log, "1@-+", "*(a", "zh1234");

  get_until_neg_test(log, "abcdefg", "12");
  get_until_neg_test(log, "abcdefg", "aA");
  get_until_neg_test(log, "abcdefg", "zyx");
  get_until_neg_test(log, "abcdefg", "ba");
}
    

void character_source_test(ttest::error_log& log) {
  std::string test {"abcd"};
  auto range = make_character_source(test, t);
  std::istringstream ss {test};
  optional<char> left;
  char right;
  while (!range.empty()) {
    left = range.get();
    if (!left) {
      log.append("get");
    }
    if (!(ss >> right)) {
      log.append("stringstream get");
    }
    if (*left != right) {
      log.append("mismatch");
    }
  }
  if (ss >> right) {
    log.append("length mismatch");
  }

  range.unget();
  range.unget();

  left = range.get();
  right = *(range.get());
  if (!range.empty()) {
    log.append("empty");
  }
  left = range.get();
  if (left) {
    log.append("empty get");
  }
}

ttest::test_suite::pointer create_character_source_test() {
  using ttest::create_test;
  return create_test("character_source", {
      create_test("get, putback", character_source_test),
      create_test("integer getting", integer_getting_test),
      create_test("get_until", get_until_test)
    });
}
