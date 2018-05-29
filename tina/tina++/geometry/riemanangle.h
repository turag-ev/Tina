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


    const Units::Angle& angle = angle_;
    const int& rieman = rieman_;

private:

    Units::Angle angle_;
    int rieman_;
};


}

#endif // RIEMANANGLE_H
