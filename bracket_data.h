#ifndef _bracket_data_h_
#define _bracket_data_h_

#include <functional>

namespace lex {

template <typename CharT, typename Traits>
class bracket_data {
 public:
  using string_type = typename Traits::string_type;
  using predicate = std::function<bool(CharT)>;
  using matcher_type = matcher<CharT>;

  bracket_data() : matching {true} {}

  void add_predicate(predicate&& p) {predicates.push_back(std::move(p));}
  void add_predicate(const predicate& p) {predicates.push_back(p);}
  void add_literal(value_type ch) {literals.push_back(ch);}
  void set_matching(bool b) {matching = b;}

  matcher_type create_matcher() const;
  void clear();
 private:
  string_type literals;
  std::vector<predicate> predicates;
  bool matching;
};
}//namespace lex
#endif// _bracket_data_h_
