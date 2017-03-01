#ifndef _syntax_option_h_
#define _syntax_option_h_

#include "regex_types.h"

namespace lex {

class syntax_option {
 public:
  using flag_type = regex_constants::syntax_option_type;

  syntax_option(flag_type f) : flag_ {f} {}
 protected:
  flag_type flag() const {return flag_;}

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
#endif// _syntax_option_h_
