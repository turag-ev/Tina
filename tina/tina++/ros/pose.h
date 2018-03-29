#ifndef TINA_ROS_POSE_H
#define TINA_ROS_POSE_H

#include <tina++/geometry.h>
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

///
/// \brief Wrapper für convert-Funktionen, der den Ausgabewert zurückgibt anstatt ihn in einen Buffer zu schreiben
/// 
/// Benutzung: 
/// ros::geometry_msgs::Pose2D in;
/// TURAG::Pose out = convert<TURAG::Pose>(in);
///
template<typename Out, typename In>
inline
Out convert(const In& in) {
    Out out;
    convert(in, out);
    return out;
}

}

#endif // TINA_ROS_POSE_H
