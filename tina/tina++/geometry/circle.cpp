#include "circle.h"

namespace TURAG {

using namespace Units;

bool intersect(const Circle& one, const Circle& two, std::array<Point, 2>& results) {
  // dx and dy are the vertical and horizontal distances between
  // the circle centers.
  Vector<Length> diff = two.m - one.m;

  // Determine the straight-line distance between the centers.
  Length d = hypot(diff.x, diff.y);

  // Check for solvability.
  if (d > (one.r + two.r) || d < abs(one.r - two.r)) {
	// no solution. circles do not intersect or one circle is contained in the other.
	return false;
  }

  // 'point 2' is the point where the line through the circle
  // intersection points crosses the line between the circle
  // centers.

  // Determine the distance from point 0 to point 2.
  Length a = (sqr(one.r) - sqr(two.r) + sqr(d)) / (2.f * d);

  /* Determine the coordinates of point 2. */
  Length x2 = one.m.x + (diff.x * a/d);
  Length y2 = one.m.y + (diff.y * a/d);

  // Determine the distance from point 2 to either of the
  // intersection points.
  Length h = sqrt(sqr(one.r) - sqr(a));

  // Now determine the offsets of the intersection points from
  // point 2.
  Length rx = -diff.y * (h/d);
  Length ry = diff.x * (h/d);

  results[0].x = x2 + rx;
  results[1].x = x2 - rx;
  results[0].y = y2 + ry;
  results[1].y = y2 - ry;
  return true;
}

} // namespace TURAG
