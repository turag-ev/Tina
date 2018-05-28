#ifndef RIEMANANGLE_H
#define RIEMANANGLE_H

#ifdef SIMULATION
#include <iostream>
#endif

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

#ifdef SIMULATION
    template<class C, class T>
    friend
    std::basic_ostream<C, T>& operator <<(std::basic_ostream<C, T>& os, const RiemanAngle ra)
    {
        return os << "[" << ra.angle_.to(Units::rad) << " rad; " << ra.rieman_ << "]";
    }
#endif

    const Units::Angle& angle = angle_;
    const int& rieman = rieman_;

private:

    Units::Angle angle_;
    int rieman_;
};


}

#endif // RIEMANANGLE_H
