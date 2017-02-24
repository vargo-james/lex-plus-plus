#ifndef _error_log_h_
#define _error_log_h_

#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace ttest {
class error_log {
 public:
  using message_type = std::basic_string<char>;
  using ostream = std::basic_ostream<char>;

  error_log(message_type name): qualifying_name_ {std::move(name)} {}

  size_t size() const {return log_.size();}
  void report(ostream& os) const;

  void append(const message_type& msg); 
  void append_if(const message_type& msg, bool fail) {
    if (fail) append(msg);
  }
  void append() {append({});}

  // This appends each message from the sublog into the log.
  void incorporate(const error_log& sublog);

 private:
  message_type qualifying_name_;
  std::vector<message_type> log_;
};

inline void error_log::append(const message_type& msg) {
  if (msg.empty()) {
    log_.push_back(qualifying_name_);
  } else {
    log_.push_back(qualifying_name_ + "::" + msg);
  }
}

inline void error_log::incorporate(const error_log& sublog) {
  for (const auto& msg: sublog.log_) {
    append(msg);
  }
}

inline void error_log::report(ostream& os) const {
  for (const auto& msg: log_) {
    os << msg << '\n';
  }
}

}//namespace ttest
#endif// _error_log_h_
