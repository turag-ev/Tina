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
  Length a = (sqr(one.r) - sqr(two.r) + sqr(d)) / (2 * d);

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

bool intersect(const Circle &zero, const Circle &one, const Circle &two, std::array<Point, 3> &results) {
  // get intersection points between each 2 circles
  std::array<Point, 2> in01, in12, in20;
  const bool ok01 = intersect(zero,  one, in01);
  const bool ok12 = intersect( one,  two, in12);
  const bool ok20 = intersect( two, zero, in20);

  if (!ok01 || !ok12 || !ok20) {
      return false;
  }

  // get intersection points that lie in all circles
  unsigned intersections = 0;
  for (const Point &candidate: {in01[0], in01[1], in12[0], in12[1], in20[0], in20[1]}) {
      if (in_range(candidate, zero, 0*mm) && in_range(candidate, one, 0*mm) && in_range(candidate, two, 0*mm)) {
          results[intersections++] = candidate;
      }
  }

  if (intersections != 3) {
      results[0] = results[1] = results[2] = {};
      return false;
  }

  return true;
}

} // namespace TURAG
