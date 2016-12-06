
#include "ttest/error_log.h"

namespace ttest {

void error_log::report(ostream& os) {
  for (const auto& msg: log_) {
    os << msg << '\n';
  }
}

void error_log::append(const message_type& msg) {
  if (msg.empty()) {
    log_.push_back(qualifying_name_);
  }
  else {
    log_.push_back(qualifying_name_ + "::" + msg);
  }
}

void error_log::aggregate(const std::vector<error_log>& sublogs) {
  for (const auto& log: sublogs) {
    incorporate(log);
  }
}

void error_log::incorporate(const error_log& sublog) {
  for (const auto& msg: sublog.log_) {
    append(msg);
  }
}
}//namespace ttest
