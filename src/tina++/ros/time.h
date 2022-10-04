#ifndef TINAPP_ROS_TIME_H
#define TINAPP_ROS_TIME_H

#include <tina++/units.h>
#include <tina++/time.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#include <ros/time.h>
#pragma GCC diagnostic pop

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

#endif // TINAPP_ROS_TIME_H
