#include "riemanangle.h"

namespace TURAG {
using namespace Units;

RiemanAngle::RiemanAngle() :
    RiemanAngle(0*rad, 0)
{ }

RiemanAngle::RiemanAngle(Angle a) :
    RiemanAngle(a, 0)
{ }

RiemanAngle::RiemanAngle(Angle a, int r) :
    angle_(a), rieman_(r)
{
    // TODO: Specify BOUNDARIES
    // TODO: Constexpr?
    while (angle_ > angle_pi) {
        angle_ -= angle_pi*2.f;
        rieman_++;
    }
    while (angle_ <= -angle_pi) {
        angle_ += angle_pi*2.f;
        rieman_--;
    }
}

bool RiemanAngle::operator ==(const RiemanAngle other) const
{
    return angle_ == other.angle_ && rieman_ == other.rieman_;
}

bool RiemanAngle::operator !=(const RiemanAngle other) const
{
    return angle_ != other.angle_ || rieman_ != other.rieman_;
}

bool RiemanAngle::operator <(const RiemanAngle other) const
{
    return rieman_ == other.rieman_ ? angle_ < other.angle_ : rieman_ < other.rieman_;
}

bool RiemanAngle::operator >(const RiemanAngle other) const
{
    return rieman_ == other.rieman_ ? angle_ > other.angle_ : rieman_ > other.rieman_;
}

bool RiemanAngle::operator <=(const RiemanAngle other) const
{
    return rieman_ == other.rieman_ ? angle_ <= other.angle_ : rieman_ <= other.rieman_;
}

bool RiemanAngle::operator >=(const RiemanAngle other) const
{
    return rieman_ == other.rieman_ ? angle_ >= other.angle_ : rieman_ >= other.rieman_;
}

}
