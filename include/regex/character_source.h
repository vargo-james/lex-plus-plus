#ifndef _character_source_h_
#define _character_source_h_

#include "data_structures/optional.h"

#include <cstddef>
#include <iterator>
#include <limits>
#include <string>

namespace lex {

template <typename BidirIt, typename Traits>
class character_source {
 public:
  using value_type = typename std::iterator_traits<BidirIt>::value_type;
  using integer_type = int;
  using string_type = typename Traits::string_type;
  using traits_type = Traits;

  character_source(BidirIt b, BidirIt e, const Traits& t) 
    : current {b}, 
      end {e}, 
      traits {t} {}

  bool empty() const {return current == end;}
  // This returns the next character in the source. It only returns an 
  // empty value if the source itself is empty.
  optional<value_type> get();
  // This is unchecked and could result in an invalid state.
  // This should only be called after a successful get() call.
  void unget() {--current;}

  // This reads digits from the source and forms them into an integral value.
  // If no digits can be read, then it returns an empty value.
  optional<std::size_t> get_int(int radix = 10);
  // This reads exactly the specified number of digits to form an integral
  // value. If that number of digits is unavailable, an empty value is 
  // returned and the source is left in its original state.
  optional<std::size_t> get_n_digit_int(std::size_t digits, int radix = 10);
  // This returns all characters in the source up to the delimiter string. Then
  // the delimiter string is discarded from the source.
  string_type get_until(const string_type& delimiter);

  const Traits& get_traits() const {return traits;}
 private:
  BidirIt current;
  BidirIt end;
  const Traits& traits;

  bool matches(const string_type& str) const;

  // This reads a value into out. It returns the number of characters read
  // from the stream. It will not read more than digit_limit characters.
  std::size_t get_int_digit_limit(std::size_t& out, std::size_t digit_limit, 
      int radix);
};

template <typename BidirIt, typename Traits>
optional<typename character_source<BidirIt,Traits>::value_type>
character_source<BidirIt,Traits>::get() {
  if (empty()) {
    return {};
  }
  return optional<value_type>(*current++);
}

template <typename BidirIt, typename Traits>
optional<std::size_t> character_source<BidirIt,Traits>::get_int(int radix) {
  std::size_t out {0};
  bool success = get_int_digit_limit(out, 
      std::numeric_limits<std::size_t>::max(), radix);

  if (success) return out;
  return {};
}

template <typename BidirIt, typename Traits>
optional<std::size_t> 
character_source<BidirIt,Traits>::get_n_digit_int(std::size_t digits, 
    int radix) {
  std::size_t out {0};
  auto count = get_int_digit_limit(out, digits, radix);
  if (count == digits) {
    return out;
  }
  std::advance(current, -count);
  return {};
}

template <typename BidirIt, typename Traits>
std::size_t character_source<BidirIt,Traits>::get_int_digit_limit(
    std::size_t& out, std::size_t digit_limit, int radix) {
  std::size_t value {0};
  std::size_t count {0};

  for (optional<value_type> ch; count < digit_limit; ++count) {
    ch = get();
    if (!ch) break;

    auto digit = traits.value(*ch, radix);
    if (digit == -1) {
      unget();
      break;
    }
    value = radix * value + digit;
  }

  if (count) out = value;
  return count;
}

template <typename BidirIt, typename Traits>
typename character_source<BidirIt,Traits>::string_type
character_source<BidirIt,Traits>::get_until(const string_type& delimiter) {
  string_type result;
  while (!empty()) {
    if (matches(delimiter)) {
      std::advance(current, delimiter.size());
      break;
    }
    else {
      result.push_back(*get());
    }
  }
  return result;
}

template <typename BidirIt, typename Traits>
bool character_source<BidirIt,Traits>::matches(const string_type& str) const {
  auto match_it = std::begin(str);
  auto match_end = std::end(str);

  for (auto it = current; it != end && match_it != match_end; 
       ++it, (void) ++match_it) {
    if (*it != *match_it) return false;
  }
  return (match_it == match_end)? true : false;
}

template <typename BidirIt, typename Traits>
character_source<BidirIt,Traits> make_character_source(BidirIt b, BidirIt e, 
    Traits& t) {
  return character_source<BidirIt,Traits>(b,e,t);
}

template <typename CharT, typename ST, typename SA, typename Traits>
character_source<typename std::basic_string<CharT,ST,SA>::const_iterator, 
  Traits>
make_character_source(const std::basic_string<CharT,ST,SA>& str, Traits& t) {
  return make_character_source(str.begin(), str.end(), t);
}

}//namespace lex
#endif// _character_source_h_
