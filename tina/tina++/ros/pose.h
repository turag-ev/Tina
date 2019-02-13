#ifndef TINA_ROS_POSE_H
#define TINA_ROS_POSE_H

#include <tina++/geometry.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Pose2D.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Accel.h>

namespace TURAG {

inline
void convert(const Pose& in, geometry_msgs::Pose2D& out) {
    out.x = in.x.to(Units::m);
    out.y = in.y.to(Units::m);
    out.theta = in.phi.to(Units::rad);
}

inline
void convert(const geometry_msgs::Pose2D& in, Pose& out) {
    out.x = in.x * Units::m;
    out.y = in.y * Units::m;
    out.phi = in.theta * Units::rad;
}

inline
void convert(const Pose& in, geometry_msgs::Pose& out) {
    out.position.x = in.x.to(Units::m);
    out.position.y = in.y.to(Units::m);
    out.position.z = 0;
    out.orientation.x = 0;
    out.orientation.y = 0;
    out.orientation.z = sin(in.phi * 0.5);
    out.orientation.w = cos(in.phi * 0.5);
}

inline
void convert(const geometry_msgs::Pose& in, Pose& out) {
    auto& q = in.orientation;
    out.x = in.position.x * Units::m;
    out.y = in.position.y * Units::m;
    out.phi = std::atan2(2 * (q.w * q.z + q.x + q.y),
                         1 - 2 * (q.y * q.y + q.z * q.z)) * Units::rad;
}

inline
void convert(const PoseVelocity& in, geometry_msgs::Twist& out) {
    out.linear.x = in.x.to(Units::m/Units::s);
    out.linear.y = in.y.to(Units::m/Units::s);
    out.linear.z = 0;
    out.angular.x = 0;
    out.angular.y = 0;
    out.angular.z = in.phi.to(Units::rad/Units::s);
}

inline
void convert(const geometry_msgs::Twist& in, PoseVelocity& out) {
    out.x = in.linear.x * (Units::m/Units::s);
    out.y = in.linear.y * (Units::m/Units::s);
    out.phi = in.angular.z * (Units::rad/Units::s);
}

inline
void convert(const PoseAcceleration& in, geometry_msgs::Accel& out) {
    out.linear.x = in.x.to(Units::m/Units::s/Units::s);
    out.linear.y = in.y.to(Units::m/Units::s/Units::s);
    out.linear.z = 0;
    out.angular.x = 0;
    out.angular.y = 0;
    out.angular.z = in.phi.to(Units::rad/Units::s/Units::s);
}

inline
void convert(const geometry_msgs::Accel& in, PoseAcceleration& out) {
    out.x = in.linear.x * (Units::m/Units::s/Units::s);
    out.y = in.linear.y * (Units::m/Units::s/Units::s);
    out.phi = in.angular.z * (Units::rad/Units::s/Units::s);
}

} // TURAG

#endif // TINA_ROS_POSE_H
