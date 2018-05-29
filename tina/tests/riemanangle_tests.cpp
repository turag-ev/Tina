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

BOOST_AUTO_TEST_SUITE_END()
