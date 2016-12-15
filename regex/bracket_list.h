#ifndef _bracket_list_h_
#define _bracket_list_h_

#include "regex_types.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace lex {
  
template <typename ElementT>
class bracket_list {
 public:
  using element_type = ElementT;
  using bracket_type = element_type;
  
  bracket_list(): matching {true} {}

  void set_matching(bool b) {matching = b;}

  void add_element(const element_type& pred) {
    elements.push_back(pred);
  }
  void add_element(element_type&& pred) {
    elements.push_back(std::move(pred));
  }

  bracket_type combine() const {return matching? any_of(): none_of();}
  
  void clear() {matching = true; elements.clear();}
 private:
  bool matching {true};
  std::vector<element_type> elements;

  element_type any_of() const;
  element_type none_of() const;
};

// This combines the element elements of a matching list.
template <typename ElementT>
typename bracket_list<ElementT>::element_type
bracket_list<ElementT>::any_of() const {
  return [els = this->elements] (const auto& ch) {
    return std::any_of(els.begin(), els.end(),
        [&ch](const auto& p) {return p(ch);});
  };
}

// This combines the element elements of a non-matching list.
template <typename ElementT>
typename bracket_list<ElementT>::element_type
bracket_list<ElementT>::none_of() const {
  return [els = this->elements] (const auto& ch) {
    return std::none_of(els.begin(), els.end(),
        [&ch](const auto& p) {return p(ch);});
  };
}

}//namespace lex
#endif// _bracket_list_h_
