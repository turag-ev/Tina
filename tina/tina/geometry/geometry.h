#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <array>

#include "extra/normalize.h"
#include <cmath>
#include "extra/units.h"

namespace SystemControl {

typedef Units::Angle Angle;
typedef Units::Length Length;

// angle functions
Angle norm_angle(Angle x);

template<typename U = Length>
struct Vector {
  U x;
  U y;

  constexpr
  Vector(U x, U y) :
    x(x), y(y)
  { }

//  constexpr
  Vector(const Vector& p) = default;
//    x(p.x), y(p.y)
//  { }

//  constexpr
  Vector() = default;
//    x(0), y(0)
//  { }
};

// point
struct Point {
  Length x;
  Length y;

  constexpr
  Point(Length x, Length y) :
    x(x), y(y)
  { }

  constexpr
  Point(const Point& p) :
    x(p.x), y(p.y)
  { }

  constexpr
  Point() :
    x(0), y(0)
  { }

  constexpr
  Vector<Length> operator+(const Point& other) {
    return Vector<Length>(x + other.x, y + other.y);
  }

  constexpr
  Vector<Length> operator-(const Point& other) {
    return Vector<Length>(x - other.x, y - other.y);
  }

  constexpr
  bool operator!=(const Point& other) {
      return (x != other.x) || (y != other.y);
  }
};

template<typename U1 = Length, typename U2 = Angle>
struct VectorPolar {
  U1 r;
  U2 phi;

  constexpr
  VectorPolar(U1 r, U2 phi) :
    r(r), phi(phi)
  { }

  constexpr
  VectorPolar(const VectorPolar& p) :
    r(p.r), phi(p.phi)
  { }

  constexpr
  VectorPolar() :
    r(), phi()
  { }
};

// pose
struct Pose {
  Length x;
  Length y;
  Angle phi;

  constexpr
  Pose(Length x, Length y, Angle phi) :
    x(x), y(y), phi(phi)
  { }

  constexpr
  Pose(const Pose& p) :
    x(p.x), y(p.y), phi(p.phi)
  { }

  constexpr
  Pose() :
    x(0), y(0), phi(0)
  { }

  explicit constexpr
  Pose(const Point& p, Angle a = 0) :
    x(p.x), y(p.y), phi(a)
  { }

  void assign(const Point& p) {
    x = p.x;
    y = p.y;
  }

  Point toPoint() const {
    return Point(x, y);
  }

  Pose& turn(Angle a) {
    phi = norm_angle(phi + a);
    return *this;
  }
  Pose& translate(Length trans);
  Pose& translate(const Point& other);
  Pose& translate(const Pose& other);

  Pose getTurnedPose(Angle a) const {
    return Pose(*this).turn(a);
  }
  Point getRelativePoint(Length trans) const;
  Point getRelativePoint(const Point& other) const;
  Point getRelativePoint(const Pose& other) const;
  Pose getRelativePose(Length trans) const {
    return Pose(*this).translate(trans);
  }
  Pose getRelativePose(const Point& other) const {
    return Pose(*this).translate(other);
  }
  Pose getRelativePose(const Pose& other) const {
    return Pose(*this).translate(other);
  }
};

////////////////////////////////////////////////////////////////////////////////

constexpr _always_inline
bool operator==(const Pose& l, const Pose& r) {
  return (l.x == r.x) && (l.y == r.y) && (l.phi == r.phi);
}

constexpr _always_inline
bool operator==(const Point& l, const Point& r) {
  return (l.x == r.x) && (l.y == r.y);
}

////////////////////////////////////////////////////////////////////////////////
// algorithm

template<typename T1, typename T2>
constexpr
Units::Area distance_sqr(const T1& a, const T2& b) {
  return sqr(a.x - b.x)  + sqr(a.y - b.y);
}

template<typename T1, typename T2>
constexpr
Length distance(const T1& a, const T2& b) {
  return sqrt(distance_sqr(a, b));
}

// special versions
constexpr Length distance(Point a, Point b) { return hypot(b.x - a.x, b.y - a.y); }
constexpr Length distance(Pose  a, Point b) { return hypot(b.x - a.x, b.y - a.y); }
constexpr Length distance(Point a, Pose  b) { return hypot(b.x - a.x, b.y - a.y); }
constexpr Length distance(Pose  a, Pose  b) { return hypot(b.x - a.x, b.y - a.y); }

template<typename T1, typename T2>
bool in_range(const T1& a, const T2& b, Length range) {
  // Umgestellt um vor Overflow zu schützen
  return sqr(a.x - b.x) <= sqr(range) - sqr(a.y - b.y);
}

template<typename T1, typename T2>
constexpr _always_inline
Angle angle_between(const T1& a, const T2& b) {
  return atan2(b.y - a.y, b.x - a.x);
}

// round
//constexpr
//int round(float x) {
//  return lround(x);
//}

constexpr _always_inline
int round(float x) {
  return (x > 0.f) ? (x + 0.5f) : (x - 0.5f);
}

////////////////////////////////////////////////////////////////////////////////
// Straight

class Straight {
public:
  template<typename T, typename U>
  constexpr explicit
  Straight(const T& t, const U& u) :
    m(static_cast<float>(u.y - t.y)/(u.x - t.x)),
    n(t.y - m * t.x)
  { }

  constexpr explicit
  Straight(float n, float m) :
    m(m),
    n(n)
  { }

  constexpr explicit
  Straight() :
    m(0.f),
    n(0.f)
  { }

  constexpr
  Length getY(Length x) const {
    return Length(m * x.value + n);
  }

  constexpr
  Length getX(Length y) const {
    return Length((y.value - n) / m);
  }

  float m;
  float n;
};

////////////////////////////////////////////////////////////////////////////////
// Circle

struct Circle {
  template<typename P>
  constexpr explicit
  Circle(const P& t, Length r) :
    m(t.x, t.y), r(r)
  { }

  constexpr explicit
  Circle() :
    m(), r(-1.f*Units::mm)
  { }

  Point m;
  Length r;
};

bool interselect(Circle one, Circle two, std::array<Point, 2>& results);

} // namespace SystemControl

#endif // GEOMETRY_H
