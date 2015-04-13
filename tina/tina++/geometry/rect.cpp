#include "rect.h"

namespace TURAG {

using namespace Units;

bool located_in(const Rect& rect, const Point& point) {
	return (point.x >= rect.getLeft()
			&& point.x <= rect.getRight()
			&& point.y >= rect.getBottom()
			&& point.y <= rect.getTop());
}

#if 1
bool in_range(const Point& pos, const Rect& rect, Length r) {
	Vector<Length> rect_half_size = rect.getSize()*0.5;
	Point rect_center = rect.getCenter();
	Vector<Length> dist = Vector<Length>(
		abs(pos.x - rect_center.x), abs(pos.y - rect_center.y));

	if (dist.x > (rect_half_size.x + r)) { return false; }
	if (dist.y > (rect_half_size.y + r)) { return false; }

	if (dist.x <= rect_half_size.x) { return true; }
	if (dist.y <= rect_half_size.y) { return true; }

	return in_range(dist, rect_half_size, r);
}
#else
bool in_range(const Point& pos, const Rect& rect, Length r) {
	//if the center of the circle is in the rectangle
	if(located_in(rect, pos))
		return true;

	Point a = rect.getLeftBottom();
	Point b = rect.getLeftTop();
	Point c = rect.getRightTop();
	Point d = rect.getRightBottom();
	struct {
		Point start;
		Vector<Length> dir;
	} edges[] = {
	  { a, b-a },
	  { b, c-b },
	  { c, d-c },
	  { d, a-d }
	};
	//calculate foot of perpendicular for each edge and check if its in range and between the endpoints
	for(const auto& e: edges) {
		//foot = start + dir*t
		float t = ((e.dir.x * (pos.x - e.start.x)) + (e.dir.y * (pos.y - e.start.y))) / (sqr(e.dir.x) + sqr(e.dir.y));
		if(t < 0.0f) {
			//foot is outside of edge (t<0), check corresponding endpoint (start)
			Length d = distance(e.start, pos);
			if(d <= r)
				return true;
		} else if(t > 1.0f) {
			//foot is outside of edge (t>1), check corresponding endpoint (end = start+dir)
			Length d = distance(e.start + e.dir, pos);
			if(d <= r) {
				return true;
			}
		} else {
			//foot is between the endpoints (t between 0 and 1), check distance
			Length d = distance(e.start + e.dir*t, pos);
			if(d <= r) {
				return true;
			}
		}
	}
	return false;
}
#endif

} // namespace TURAG
