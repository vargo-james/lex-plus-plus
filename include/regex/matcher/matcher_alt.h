#ifndef _regex_matcher_h_
#define _regex_matcher_h_

#include "data_structures/forward_iterator.h"
#include "data_structures/optional.h"
#include "match_results.h"
#include "regex_types.h"

namespace lex {
template <typename CharT, typename Traits>
class matcher : regex_constants {
 public:
  using value_type = CharT;
  using string_type = typename Traits::string_type;
  using iterator = forward_iterator<CharT, const CharT&>;
  using regex_constants::syntax_option_type;

  matcher(syntax_option_type syntax, const Traits& tr)
    : syntax_ {syntax},
      traits_ {tr} {}

  virtual optional<iterator> match(match_results<string_type>& results, 
      iterator begin, iterator end) {return {};}
  virtual optional<iterator> next_match(match_results<string_type>& results,
      iterator begin, iterator end) {return {};}
  virtual void update(match_results<string_type>& results, CharT next_char) {}

  virtual ~matcher() = default;
 protected:
  syntax_option_type syntax() const {return syntax_;}
  optional<CharT> prev() const {return prev_;}
  const Traits& traits() const {return traits_;}

  void initialize(CharT ch);
  void initialize() {initialize_state();}
 private:
  syntax_option_type syntax_;
  const Traits& traits_;
  optional<CharT> prev_;

  virtual void initialize_state() {}
};

template <typename CharT, typename Traits>
void matcher<CharT, Traits>::initialize(CharT ch) {
  prev_ = ch;
  initialize_state();
}

/* ************************************************************************* */

template <typename CharT, typename Traits>
class left_anchor_matcher : public matcher<CharT,Traits> {
 public:
  using typename matcher<CharT,Traits>::iterator;
  using typename matcher<CharT,Traits>::string_type;
  using typename matcher<CharT,Traits>::syntax_option_type;

  using matcher<CharT,Traits>::matcher;

  optional<iterator> match(match_results<string_type>& results,
      iterator begin, iterator end) override;
  optional<iterator> next_match(match_results<string_type>& results,
      iterator begin, iterator end) override;

  void update(match_results<string_type>& results, CharT next_char) override {
    full = true;
  }

 private:
  bool full {false};
  bool matches() const;
  void initialize_state() override {full = false;}
};

template <typename CharT, typename Traits>
optional<typename left_anchor_matcher<CharT,Traits>::iterator>
left_anchor_matcher<CharT,Traits>::match(match_results<string_type>& results,
    iterator begin, iterator end) {
  return next_match(results, begin, end);
}

template <typename CharT, typename Traits>
optional<typename left_anchor_matcher<CharT,Traits>::iterator>
left_anchor_matcher<CharT,Traits>::next_match(
    match_results<string_type>& results, iterator begin, iterator end) {
  if (!full && matches()) {
    return begin;
  }
  return {};
}

template <typename CharT, typename Traits>
bool left_anchor_matcher<CharT,Traits>::matches() const {
  if (!(this->prev())) return true;
  return  this->prev() == CharT('\n') &&
         (this->syntax() & syntax_option_type::multiline) && 
         (this->syntax() & syntax_option_type::ECMAScript);
}

/* ************************************************************************* */

template <typename CharT, typename Traits>
class string_matcher : public matcher<CharT, Traits> {
 public:
  using typename matcher<CharT,Traits>::value_type;
  using typename matcher<CharT,Traits>::iterator;
  using typename matcher<CharT,Traits>::string_type;
  using typename matcher<CharT,Traits>::syntax_option_type;

  string_matcher() = default;
  string_matcher(string_type str, syntax_option_type syntax, Traits& tr)
    : matcher<CharT,Traits>(syntax, tr),
      pattern {std::move(str)} {}

  optional<iterator> match(match_results<string_type>& results, iterator begin, 
      iterator end) override;
 private:
  const string_type pattern;
  std::size_t index {0};
};

template <typename CharT, typename Traits>
void string_matcher<CharT,Traits>::initialize_state() {
  index = 0;
}

template <typename CharT, typename Traits>
void string_matcher<CharT,Traits>::update(
    match_results<string_type>& results, CharT ch) {
  if (ch == pattern[index]) {
    ++index;
  }
}

template <typename CharT, typename Traits>
void string_matcher<CharT,Traits>::next_match(
    match_results<string_type>& results, iterator begin, iterator end) {
  iterator input_iter(begin);
  for (auto it = pattern.begin() + index; 
       it != pattern.end() && input_iter != end; 
       ++it, (void) ++input_iter) {
    if (*it != *input_iter) {
      return {};
    }
  }
  results.append(begin, input_iter);
  return input_iter;
}
template <typename CharT, typename Traits>
optional<typename string_matcher<CharT,Traits>::iterator>
string_matcher<CharT,Traits>::match(match_results<string_type>& results, 
    iterator begin, iterator end) {
  iterator input_iter(begin);
  for (auto it = pattern.begin(); 
       it != pattern.end() && input_iter != end; 
       ++it, (void) ++input_iter) {
    if (*it != *input_iter) {
      return {};
    }
  }
  results.append(begin, input_iter);
  return input_iter;
}

}//namespace lex

#endif// _regex_matcher_h_
