#ifndef _markup_table_h_
#define _markup_table_h_

#include "optional.h"

#include <initializer_list>
#include <functional>
#include <vector>

namespace lex {
template <typename CharT, typename Mark>
struct marked_value {
  using char_type = CharT;
  using mapped_type = Mark;

  char_type value;
  mapped_type type;
};

template <typename CharT, typename Mark>
class markup_table {
 public:
  using char_type = CharT;
  using mapped_type = Mark;
  using entry_type = marked_value<CharT,Mark>;
  using table_type = std::vector<entry_type>;
  using predicate_type = std::function<bool(char_type)>;

  markup_table() {}

  markup_table(const std::initializer_list<entry_type>& init,
      const predicate_type& pred, mapped_type if_pred, mapped_type def_val)
    : table_(init)
    , predicate {pred}
    , if_predicate {if_pred}
    , default_val {def_val} {}

  markup_table(const std::initializer_list<entry_type>& init,
      mapped_type def_val)
    : table_(init)
    , default_val {def_val} {}

  markup_table(const std::initializer_list<entry_type>& init)
    : table_(init) {}

  markup_table(const markup_table& other) = default;
  markup_table(markup_table&& other) = default;

  markup_table& operator=(const markup_table& other) = default;
  markup_table& operator=(markup_table&& other) = default;

  optional<mapped_type> lookup(char_type val) const;
  bool includes(char_type val) const {
    return table_contains(val) || predicate(val);
  }

  void insert(char_type val, mapped_type label) {insert({val, label});}

 private:
  table_type table_;
  predicate_type predicate {[](auto){return false;}};
  optional<mapped_type> if_predicate;
  optional<mapped_type> default_val;

  void insert(entry_type entry) {table_.push_back(entry);}
  bool apply_predicate(char_type val) const {return predicate(val);}
  bool table_contains(char_type val) const;
};

template <typename CharT, typename Mark>
bool markup_table<CharT,Mark>::table_contains(char_type val) const {
  for (const auto& entry : table_) {
    if (entry.value == val) return true;
  }
  return false;
}

template <typename CharT, typename Mark>
optional<Mark> markup_table<CharT,Mark>::lookup(char_type val) const {
  for (const auto& entry : table_) {
    if (entry.value == val) {
      return entry.type;
    }
  }
  if (predicate(val)) {
    return if_predicate;
  }

  return default_val;
}


}//namespace lex
#endif// _markup_table_h_
