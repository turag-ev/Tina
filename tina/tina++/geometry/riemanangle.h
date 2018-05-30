#ifndef RIEMANANGLE_H
#define RIEMANANGLE_H

#include "../units.h"

namespace TURAG {

struct RiemanAngle {
    RiemanAngle();
    RiemanAngle(Units::Angle a);
    RiemanAngle(Units::Angle a, int r);

    bool operator ==(const RiemanAngle other) const;
    bool operator !=(const RiemanAngle other) const;
    bool operator <(const RiemanAngle other) const;
    bool operator >(const RiemanAngle other) const;
    bool operator <=(const RiemanAngle other) const;
    bool operator >=(const RiemanAngle other) const;

    RiemanAngle operator +(const RiemanAngle other) const;
    RiemanAngle operator -(const RiemanAngle other) const;

    /// Return the angle in the interval (-pi, pi]
    Units::Angle norm_angle() const;

    /// Return the full angle including all 360deg rotations
    Units::Angle full_angle() const;

    /// For testing purposes only! Use norm_angle() or full_angle()!
    const Units::Angle& angle = angle_;

    /// Number of full rotations in this angle
    const int& rieman = rieman_;

private:
    Units::Angle angle_;
    int rieman_;
};


}

#endif // RIEMANANGLE_H
