#ifndef CONVERT_H
#define CONVERT_H

#include "pose.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#include "time.h"
#pragma GCC diagnostic pop

namespace TURAG {

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

} // TURAG

#endif // CONVERT_H
