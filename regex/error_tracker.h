#ifndef _error_tracker_h_
#define _error_tracker_h_

#include "regex_types.h"

namespace lex {

class error_tracker {
 public:
  using error_type = regex_constants::error_type;
  error_tracker(error_type& e): ec {e} {}

  error_type error() const {return ec;}
  void set_error(error_type e) {ec = e;}

  bool no_error() const {return ec == error_type::error_none;}
 protected:
  error_type& get_error() {return ec;}
 private:
  error_type& ec;
};

}//namespace lex
#endif// _error_tracker_h_
