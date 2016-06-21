#ifndef _regex_expression_types_h_
#define _regex_expression_types_h_

#include <functional>
#include <iterator>

namespace token_iterator {
namespace detail {

template <typename Iterator>
using char_type_t = typename std::iterator_traits<Iterator>::value_type;

template <typename Char>
using predicate_type_t = std::function<bool(const Char&)>;

}//namespace detail
}//namespace token_iterator

#endif// _regex_expression_types_h_
