#include "cost.h"

namespace TURAG {

Cost Cost::operator+=(unsigned arg) {
  if (cost_ < MAX_) {
    if (arg > MAX_ - cost_) {
      // overflow
      cost_ = (arg == INFINITY_) ? INFINITY_ : MAX_;
    } else {
      cost_ += arg;
    }

  } else if (arg == INFINITY_) {
    cost_ = INFINITY_;
  }
  return *this;
}

Cost Cost::operator-=(unsigned arg) {
  if (cost_ <= MAX_) {
    cost_ = (arg > cost_) ? 0 : (cost_ - arg);
  }
  return *this;
}

} // namespace TURAG
