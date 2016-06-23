#ifndef _pattern_table_h_
#define _pattern_table_h_

#include "regex_expression.h"

template <typename Char, typename T>
class pattern_table {
 public:
  using char_type = Char;
  using regex_type = simple_regex<char_type>;
  using string_type = std::basic_string<char_type>;
  using constructor = std::function<T(const string_type<char_type>&)>;
  using table_type = std::vector<regex_type, constructor>; 
  using const_iterator = typename table::const_iterator;
  using iterator = typename table::iterator;;

  template <typename Iterator>
  pattern_table(Iterator begin, Iterator end) : table(begin, end) {}

  pattern_table(const std::initializer_list<string_type, constructor>& l)
    : pattern_table(l.begin(), l.end()) {}
  pattern_table(const std::vector<string_type, constructor>& v)
    : pattern_table(l.begin(), l.end()) {}

  iterator begin() const {return table.begin();} 
  iterator end() const {return table.end();}
 private:
  table_type table;
}

#endif// _pattern_table_h_
