#include "line.h"

namespace TURAG {

namespace {
    enum class Orientation {
        CLOCKWISE,
        COUNTERCLOCKWISE,
        COLINEAR
    };
    
    Orientation getOrientation(const Point& a, const Point& b, const Point& c) {
        // https://www.geeksforgeeks.org/orientation-3-ordered-points/
        const auto slope = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);
        return slope == Units::null ? Orientation::COLINEAR :
               slope > Units::null ? Orientation::CLOCKWISE :
                                     Orientation::COUNTERCLOCKWISE;
    }
    
    constexpr bool pointIsOnLine(const Point& p, const Line& l) {
        // assume that l.p, l.q and p are colinear
        return p.x <= l.getRight() && p.x >= l.getLeft()
                && p.y <= l.getTop() && p.y >= l.getBottom();
    }
    
    constexpr Units::Area squaredProjectionDistance(const Line& l, const Point& p) {
        // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
        return sqr((l.q.y - l.p.y) * p.x - (l.q.x - l.p.x) * p.y + l.q.x * l.p.y - l.q.y * l.p.x) / l.getSquaredLength();
    }
}

bool intersect(const Line& a, const Line& b) {
    // https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
    
    const Orientation o1 = getOrientation(a.p, a.q, b.p);
    const Orientation o2 = getOrientation(a.p, a.q, b.q);
    const Orientation o3 = getOrientation(b.p, b.q, a.p);
    const Orientation o4 = getOrientation(b.p, b.q, a.q);
    
    if (o1 != o2 && o3 != o4)
        return true;
    
    if (o1 == Orientation::COLINEAR && pointIsOnLine(b.p, a))
        return true;
    
    if (o2 == Orientation::COLINEAR && pointIsOnLine(b.q, a))
        return true;
    
    if (o3 == Orientation::COLINEAR && pointIsOnLine(a.p, b))
        return true;
    
    if (o4 == Orientation::COLINEAR && pointIsOnLine(a.q, b))
        return true;
    
    return false;
}

Units::Area distance_sqr(const Line& l, const Point& r) {
    using Units::Area;
    
    // s ist Projektionspunkt von r auf Gerade durch p und q
    const Area sr = squaredProjectionDistance(l, r);
    
    const Area pq = l.getSquaredLength();
    const Area pr = distance_sqr(l.p, r);
    const Area qr = distance_sqr(l.q, r);
    
    // Prüfe, ob p oder q weiter von r weg sind
    if (pr > qr) {
        // Bestimme Abstand zwischen p und s
        const Area ps = pr - sr;
        if (ps > pq)
            // s liegt außerhalb der Linie zwischen p und q
            return qr;
        else
            return sr;
    } else {
        // Bestimme Abstand zwischen q und s
        const Area qs = qr - sr;
        if (qs > pq)
            // s liegt außerhalb der Linie zwischen p und q
            return pr;
        else
            return sr;
    }
}

} // namespace TURAG
