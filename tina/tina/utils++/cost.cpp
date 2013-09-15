#include "cost.h"

Cost& Cost::operator+=(unsigned arg) {
  if (cost_ < MAX_) {
    if (arg > MAX_ - cost_) {
      // overflow COST_MAX
      cost_ = MAX_;
    } else {
      cost_ += arg;
    }
  }
  return *this;
}

Cost& Cost::operator-=(unsigned arg) {
  if (cost_ < MAX_) {
    cost_ = (arg > cost_) ? 0 : cost_ - arg;
  }
  return *this;
}

Cost& Cost::operator+=(int arg) {
  if (arg >= 0) {
    return *this += unsigned(arg);
  } else {
    return *this -= unsigned(-arg);
  }
  return *this;
}
