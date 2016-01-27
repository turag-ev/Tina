#include "pie.h"

namespace TURAG {

    bool intersect(Pie& pie, Circle& circle) {
        if(distance(pie.m,circle.m) > (pie.r + circle.r))
            return false;


        //calculate angles and move to [0,2pi] range
        Angle phi = norm_angle(angle_between(circle.m, pie.m) );
        if(phi < 0*Units::deg)
            phi += Units::angle_pi;
        Angle start = norm_angle(pie.start);
        if(start < 0*Units::deg)
            start += Units::angle_pi;

        //check if angle between center points lies within range
        return abs(phi - start) < pie.width;
    }

} // naemsapce TURAG
