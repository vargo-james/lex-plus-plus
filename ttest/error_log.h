#ifndef _error_log_h_
#define _error_log_h_

#include <ostream>
#include <string>
#include <vector>

namespace ttest {
class error_log {
 public:
  using message_type = std::string;
  using ostream = std::ostream;

  error_log(const message_type& name): qualifying_name_ {name} {}

  size_t size() const {return log_.size();}
  void report(ostream& os) const;

  void append(const message_type& msg);
  void append_if(const message_type& msg, bool fail) {if (fail) append(msg);}
  void append() {append({});}
  // This appends each message from the sublog into the log.
  void incorporate(const error_log& sublog);

  void aggregate(const std::vector<error_log>& sublogs);
 
 private:
  message_type qualifying_name_;
  std::vector<message_type> log_;

};
}//namespace ttest
#endif// _error_log_h_
