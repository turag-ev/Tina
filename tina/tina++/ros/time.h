#ifndef TIME_H
#define TIME_H

#include <tina++/units.h>
#include <tina++/time.h>
#include <ros/time.h>

namespace TURAG {

inline
void convert(const Units::Time& in, ros::Time& out) {
    out.fromSec(in.to(Units::s));
}

inline
void convert(const ros::Time& in, Units::Time& out) {
    out = in.toSec() * Units::s;
}

// TODO: TURAG::SystemTime <--> ros::Time

} // TURAG

#endif // TIME_H
