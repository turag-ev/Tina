#ifndef COST_H
#define COST_H

#include <limits>
#include "extra/normalize.h"

class Cost {
public:
  static constexpr unsigned INFINITY_ = std::numeric_limits<unsigned>::max();
  static constexpr unsigned MAX_ = INFINITY_ - 1;

  constexpr
  Cost() :
    cost_(INFINITY_)
  { }

  constexpr explicit
  Cost(unsigned arg) :
    cost_(arg)
  { }

  Cost& operator+=(int arg);
  Cost& operator+=(unsigned arg);

  Cost& operator+=(Cost other) {
    return *this += other.cost_;
  }

  Cost& operator-=(int arg) {
    return *this += -arg;
  }
  Cost& operator-=(unsigned arg);

  constexpr bool operator< (Cost other)     { return cost_ <  other.cost_; }
  constexpr bool operator< (unsigned other) { return cost_ <  other;       }
  constexpr bool operator> (Cost other)     { return cost_ >  other.cost_; }
  constexpr bool operator> (unsigned other) { return cost_ >  other;       }
  constexpr bool operator<=(Cost other)     { return cost_ <= other.cost_; }
  constexpr bool operator<=(unsigned other) { return cost_ <= other;       }
  constexpr bool operator>=(Cost other)     { return cost_ >= other.cost_; }
  constexpr bool operator>=(unsigned other) { return cost_ >= other;       }
  constexpr bool operator==(Cost other)     { return cost_ == other.cost_; }
  constexpr bool operator==(unsigned other) { return cost_ == other;       }
  constexpr bool operator!=(Cost other)     { return cost_ != other.cost_; }
  constexpr bool operator!=(unsigned other) { return cost_ != other;       }

  constexpr explicit
  operator unsigned() const {
    return cost_;
  }

private:
  unsigned cost_;
};

/// unendlich große Kosten (Weg nicht benutzbar)
static constexpr Cost COST_INFINITY = Cost(Cost::INFINITY_);

/// größte Kosten ohne ausgeschlossen zu werden
static constexpr Cost COST_MAX = Cost(Cost::INFINITY_ - 1);

#endif // COST_H
