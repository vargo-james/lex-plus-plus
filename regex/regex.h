/*
 *  The regex class defined here has almost the same interface
 *  as the standard basic_regex class.
 */

#ifndef _regex_h_
#define _regex_h_

#include "reader.h"

#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <regex>
#include <string>

namespace lex {

template <typename CharT, typename Traits = std::regex_traits<CharT>>
class regex {
 public:
  using value_type = CharT;
  using traits_type = Traits;
  using string_type = typename Traits::string_type;
  using locale_type = typename Traits::locale_type;
  using flag_type = std::regex_constants::syntax_option_type;

  //Constructors
  regex() = default;
  regex(const regex& other) = default;
  regex(regex&& other) = default;

  explicit regex(const CharT* s, flag_type f = std::regex_constants::extended)
    : regex(s, s + strlen(s), f) {}
  regex(const CharT* s, std::size_t n, 
      flag_type f = std::regex_constants::extended)
    : regex(s, s + n, f) {}
  template <typename ST, typename SA>
  explicit regex(const std::basic_string<CharT,ST,SA>& str,
      flag_type f = std::regex_constants::extended) 
    : regex(std::begin(str), std::end(str), f) {}
  regex(const std::initializer_list<CharT>& init, 
      flag_type f = std::regex_constants::extended)
    : regex(std::begin(init), std::end(init), f) {}
  template <typename ForwardIt>
  regex(ForwardIt first, ForwardIt last, 
      flag_type f = std::regex_constants::extended);

  // Destructor
  ~regex() = default;

  // Operator=
  regex& operator=(const regex& other) = default;
  regex& operator=(regex&& other) = default;
  regex& operator=(CharT* s) {return this->assign(s);}
  regex& operator=(const std::initializer_list<CharT>& init) {
    return this->assign(init);
  }
  template <typename ST, typename SA>
  regex& operator=(const std::basic_string<CharT,ST,SA>& str) {
    return this->assign(str);
  }

  // Assign
  regex& assign(const regex& other) {return *this = other;}
  regex& assign(regex&& other) {return *this = std::move(other);}
  regex& assign(const CharT* s, flag_type f = std::regex_constants::extended) {
    return *this = regex(s, f);
  }
  regex& assign(const CharT* s, std::size_t n, 
      flag_type f = std::regex_constants::extended) {
    return *this = regex(s,n,f);
  }
  template <typename ST, typename SA>
  regex& assign(const std::basic_string<CharT,ST,SA>& str,
      flag_type f = std::regex_constants::extended) {
    return *this = regex(str, f);
  }
  template <typename InputIt>
  regex& assign(InputIt first, InputIt last, 
      flag_type f = std::regex_constants::extended) {
    return *this = regex(std::basic_string<CharT>(first, last), f);
  }
  regex& assign(const std::initializer_list<CharT>& init,
      flag_type f = std::regex_constants::extended) {
    return *this = regex(init, f);
  }

  // Observers
  flag_type flags() const {return f_;}
  unsigned mark_count() const {return 0;}

  // Locale
  locale_type getloc() const {return traits_i.getloc();}
  locale_type imbue(locale_type loc) {return traits_i.imbue(loc);}

  template <typename ForwardIt>
  ForwardIt match(ForwardIt begin, ForwardIt end);
 private:
  matcher<CharT> matcher_;
  flag_type f_;
  locale_type traits_i;
};

template <typename CharT, typename Traits>
template <typename ForwardIt>
regex<CharT,Traits>::regex(ForwardIt first, ForwardIt last, flag_type f) 
  : f_ {f} {
  reader<ForwardIt,Traits> reader(first, last, f);
  matcher_ = reader.read();
}

template <typename CharT, typename Traits>
template <typename ForwardIt>
ForwardIt regex<CharT,Traits>::match(ForwardIt begin, ForwardIt end) {
  auto accepted = begin;
  bool loop_done {false};
  for (auto seek = begin; !loop_done && seek != end;) {
    matcher_.update(*seek);
    ++seek;
    switch (matcher_.state()) {
    case match_state::MATCH:
      accepted = seek;
      break;
    case match_state::FINAL_MATCH:
      accepted = seek;
      loop_done = true;
      break;
    case match_state::MISMATCH:
      loop_done = true;
      break;
    case match_state::UNDECIDED:
      break;
    }
  }
  // The matcher needs to be reinitialized to be used again later.
  matcher_.initialize();
  return accepted;
}
         
}//namespace lex
#endif// _regex_h_
