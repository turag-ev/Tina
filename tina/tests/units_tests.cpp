#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <tina++/geometry/units.h>
#include <tina++/units/electronics.h>

using namespace TURAG;
using namespace TURAG::Units;

BOOST_AUTO_TEST_SUITE(UnitsTests)

BOOST_AUTO_TEST_CASE( test_Unit_toInt ) {
    Length s2 = 700 * mm;

    BOOST_CHECK_EQUAL(s2.toInt(m), 1);
}

BOOST_AUTO_TEST_CASE( test_Unit_power ) {
    BOOST_CHECK_EQUAL((power<2>(2*mm)).to(mm*mm), 4);
    BOOST_CHECK_EQUAL((power<2,1>(2*mm)).to(mm*mm), 4);

    BOOST_CHECK_EQUAL((root<2>(4*(mm*mm))).to(mm), 2);
    BOOST_CHECK_EQUAL((root<2,1>(4*(mm*mm))).to(mm), 2);
    BOOST_CHECK_EQUAL((power<1,2>(4*(mm*mm))).to(mm), 2);
}

BOOST_AUTO_TEST_CASE( test_Geometry )
{
    // x * x = A
    BOOST_CHECK_EQUAL(((2*mm) * (2*mm)).to(mm*mm), 4);

    // v = s / t
    BOOST_CHECK_EQUAL(((4*m) / (2*s)).to(m/s), 2);

    // \omega = \alpha / t
    BOOST_CHECK_EQUAL(((4*deg) / (2*s)).to(deg/s), 2);
}

BOOST_AUTO_TEST_CASE( test_Physics )
{
    // F = m * a
    BOOST_CHECK_EQUAL(((1*kg) * (10*(m/(s*s)))).toInt(N), 10);

    // M = F * r
    BOOST_CHECK_EQUAL(((5*N) * (0.5*m)).to(Nm), 2.5);

    // R = U / I
    BOOST_CHECK_EQUAL(((5*V) / (2*A)).to(Ohm), 2.5);

    // P = U * I
    BOOST_CHECK_EQUAL(((6*V) * (7*A)).to(W), 42);

    // F = l * I * B * sin \alpha
    BOOST_CHECK_CLOSE(((1*m) * (100*mA) * (1*T) * (sin(90*deg))).to(N), 0.1, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
