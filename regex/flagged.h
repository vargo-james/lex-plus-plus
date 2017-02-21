#ifndef _flagged_h_
#define _flagged_h_

#include "regex_types.h"

#include <utility>

namespace lex {

template <typename Base, typename Flag>
class flagged : public Base {
 public:
  using flag_type = Flag;
  template <typename... Args>
  flagged(flag_type f, Args&&... args)
   : Base(std::forward<Args>(args)...)
   , flag_(f) {}
     
  template <typename U, typename... Args>
  flagged(const flagged<U,Flag>& first, Args&&... args)
   : flagged(first.flag(), first, std::forward<Args>(args)...) {}

  template <typename U, typename... Args>
  flagged(const flagged<U,Flag>&& first, Args&&... args)
   : flagged(first.flag(), std::move(first), std::forward<Args>(args)...) {}

  flag_type flag() const {return flag_;}
 protected:

  bool icase() const {return flag_ & flag_type::icase;}
  bool basic() const {return flag_ & flag_type::basic;}
  bool extended() const {return flag_ & flag_type::extended;}
  bool grep() const {return flag_ & flag_type::grep;}
  bool egrep() const {return flag_ & flag_type::egrep;}
  bool ECMAScript() const {return flag_ & flag_type::ECMAScript;}

 private:
  flag_type flag_;
};

}//namespace lex
#endif// _flagged_h_
