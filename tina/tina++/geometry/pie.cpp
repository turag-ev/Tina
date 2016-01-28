#include "pie.h"
#include <tina++/debug.h>

namespace TURAG {
    //assumes angles are [0,360] deg
    bool between_angles(const Angle& start, const Angle& width, const Angle& phi) {
        if(phi < start)
            return (start+width - 360*Units::deg) > phi;
        else
            return phi < start + width;
    }

    bool intersect(const Pie& pie, const Circle& circle) {
        Length dist = distance(pie.m, circle.m);
        if(dist > (pie.r + circle.r))
            return false;

        //Angle between tangent and center connection
        Angle delta = asin(circle.r.to(Units::mm) / dist.to(Units::mm)) * Units::rad;

        //Angle of vector between centers
        Angle phi = norm_angle(angle_between(circle.m, pie.m));

        //Angles of tangent vectors
        Angle theta0 = norm_angle(phi + delta);
        Angle theta1 = norm_angle(phi - delta);

        //extra variable to prevent side-effects even though there shouldn't be any ;>
        Angle start = norm_angle(pie.start);
        //Transform angles into [0-360] deg range
        if(theta0 < 0*Units::deg)
            theta0 += 180*Units::deg;
        if(theta1 < 0*Units::deg)
            theta1 += 180*Units::deg;
        if(start < 0*Units::deg)
            start += 180*Units::deg;
        if(phi < 0*Units::deg)
            phi += 180*Units::deg;

        //check if one of the vectors hits the pie
        return (between_angles(start,pie.width,phi) ||
                between_angles(start,pie.width,theta0) ||
                between_angles(start,pie.width,theta1) );
    }

} // naemsapce TURAG
