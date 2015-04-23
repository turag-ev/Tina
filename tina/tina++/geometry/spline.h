#ifndef SPLINE_H
#define SPLINE_H

#include "geometry.h"

namespace TURAG {

/// \brief Spline
///
template<std::size_t order>
class Spline
{
public:
    Spline();
    ~Spline();

    constexpr int calc() const { return order*2;}

private:
    static constexpr std::size_t Order = order;

    float cx[order+1];
    float cy[order+1];
    float direct_dist;
    float length;
    float kappa_max;
};

using Spline5 = Spline<5>;
using Spline3 = Spline<3>;

} // namespace TURAG

#endif // SPLINE_H
