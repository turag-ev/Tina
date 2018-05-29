#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <tina++/geometry/riemanangle.h>

using namespace TURAG;
using namespace Units;

static constexpr float FLOAT_EQ_TOL = 0.001;

BOOST_AUTO_TEST_SUITE(RiemanAngleTests)

BOOST_AUTO_TEST_CASE(Constructors) {
    // Default constructor
    const RiemanAngle a;
    BOOST_CHECK_EQUAL(a.angle.to(rad), 0.f);
    BOOST_CHECK_EQUAL(a.rieman, 0);

    // Angle only
    const RiemanAngle b(90*deg);
    BOOST_CHECK_EQUAL(b.angle.to(deg), 90.f);
    BOOST_CHECK_EQUAL(b.rieman, 0);

    // Angle + rieman
    const RiemanAngle c(90*deg, 1);
    BOOST_CHECK_EQUAL(c.angle.to(deg), 90.f);
    BOOST_CHECK_EQUAL(c.rieman, 1);

    // Huge angle
    const RiemanAngle e(360*deg + 90*deg);
    BOOST_CHECK_CLOSE(e.angle.to(rad), M_PI_2f, FLOAT_EQ_TOL);
    BOOST_CHECK_EQUAL(e.rieman, 1);

    // Huge neg. angle
    const RiemanAngle f(-360*deg);
    BOOST_CHECK_EQUAL(f.angle.to(rad), 0.f);
    BOOST_CHECK_EQUAL(f.rieman, -1);

    // Huge anlge + rieman
    const RiemanAngle g(360*deg + 90*deg, 1);
    BOOST_CHECK_CLOSE(g.angle.to(rad), M_PI_2f, FLOAT_EQ_TOL);
    BOOST_CHECK_EQUAL(g.rieman, 2);

    // Copy constructor
    const RiemanAngle h(c);
    BOOST_CHECK_EQUAL(h.angle.to(deg), 90.f);
    BOOST_CHECK_EQUAL(h.rieman, 1);

    // Assignment
    const RiemanAngle i = c;
    BOOST_CHECK_EQUAL(i.angle.to(deg), 90.f);
    BOOST_CHECK_EQUAL(i.rieman, 1);
}

BOOST_AUTO_TEST_CASE(ComparisonsEqual) {
    // Empty angle
    const RiemanAngle a;
    BOOST_CHECK(a == RiemanAngle());
    BOOST_CHECK(!(a != RiemanAngle()));

    // Angle only
    const RiemanAngle b(90*deg);
    BOOST_CHECK(a != b);
    BOOST_CHECK(!(a == b));

    const RiemanAngle c(90*deg);
    BOOST_CHECK(b == c);
    BOOST_CHECK(!(b != c));

    // Angle with rieman level 1
    const RiemanAngle d(90*deg, 1);
    BOOST_CHECK(c != d);
    BOOST_CHECK(!(c == d));

    const RiemanAngle e(90*deg, 1);
    BOOST_CHECK(e == d);
    BOOST_CHECK(!(e != d));
}

BOOST_AUTO_TEST_CASE(ComparisonsLessGreater) {
    // Simple angles
    const RiemanAngle a(60*deg);
    const RiemanAngle b(80*deg);

    BOOST_CHECK(b > a);
    BOOST_CHECK(a < b);
    BOOST_CHECK(b >= a);
    BOOST_CHECK(a <= b);
    BOOST_CHECK(!(a > b));
    BOOST_CHECK(!(b < a));
    BOOST_CHECK(!(a >= b));
    BOOST_CHECK(!(b <= a));

    const RiemanAngle c(60*deg);
    BOOST_CHECK(a >= c);
    BOOST_CHECK(a <= c);
    BOOST_CHECK(!(a > c));
    BOOST_CHECK(!(a < c));

    // Angle with rieman
    const RiemanAngle d(60*deg, 1);

    BOOST_CHECK(d > a);
    BOOST_CHECK(a < d);
    BOOST_CHECK(d >= a);
    BOOST_CHECK(a <= d);
    BOOST_CHECK(!(a > d));
    BOOST_CHECK(!(d < a));
    BOOST_CHECK(!(a >= d));
    BOOST_CHECK(!(d <= a));
}

BOOST_AUTO_TEST_CASE(Add) {
    // Angle only + angle only
    RiemanAngle a(60*deg);
    RiemanAngle b(40*deg);

    RiemanAngle c = a + b;
    BOOST_CHECK_EQUAL(c.angle.to(deg), 100.f);
    BOOST_CHECK_EQUAL(c.rieman, 0);

    // Implicit Angle to RiemanAngle conversion
    RiemanAngle d = a + 10*deg;
    BOOST_CHECK_EQUAL(d.angle.to(deg), 70.f);
    BOOST_CHECK_EQUAL(d.rieman, 0);

    // Would this be intended behavior?
    //RiemanAngle e = 10*deg + a;
    //BOOST_CHECK_EQUAL(e.angle.to(deg), 70.f);
    //BOOST_CHECK_EQUAL(e.rieman, 0);

    // Angle with rieman + angle with rieman
    RiemanAngle f(20*deg, 2);
    RiemanAngle g(30*deg, 3);
    RiemanAngle h = f + g;
    BOOST_CHECK_EQUAL(h.angle.to(deg), 50.f);
    BOOST_CHECK_EQUAL(h.rieman, 5);

    // Increment rieman level
    RiemanAngle i(170*deg);
    RiemanAngle j = i + 180*deg;
    BOOST_CHECK_CLOSE(j.angle.to(deg), -10.f, FLOAT_EQ_TOL);
    BOOST_CHECK_EQUAL(j.rieman, 1);
}

BOOST_AUTO_TEST_CASE(Sub) {
    RiemanAngle a(60*deg);
    RiemanAngle b(40*deg);

    RiemanAngle c = a - b;
    BOOST_CHECK_CLOSE(c.angle.to(deg), 20.f, FLOAT_EQ_TOL);
    BOOST_CHECK_EQUAL(c.rieman, 0);

    RiemanAngle d = a - 10*deg;
    BOOST_CHECK_CLOSE(d.angle.to(deg), 50.f, FLOAT_EQ_TOL);
    BOOST_CHECK_EQUAL(d.rieman, 0);

    RiemanAngle e(20*deg, 2);
    RiemanAngle f(30*deg, 3);
    RiemanAngle g = e - f;
    BOOST_CHECK_CLOSE(g.angle.to(deg), -10.f, FLOAT_EQ_TOL);
    BOOST_CHECK_EQUAL(g.rieman, -1);

    RiemanAngle h = e - 360*deg;
    BOOST_CHECK_CLOSE(h.angle.to(deg), 20.f, FLOAT_EQ_TOL);
    BOOST_CHECK_EQUAL(h.rieman, 1);
}

BOOST_AUTO_TEST_SUITE_END()
