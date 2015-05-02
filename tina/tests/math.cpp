#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <limits>

#include <tina++/math.h>

using namespace TURAG;

BOOST_AUTO_TEST_SUITE(MathTests)

BOOST_AUTO_TEST_CASE( test_float_equal ) {
    // true things
    BOOST_CHECK_EQUAL(float_equal(0.0f, 0.0f), true);
    BOOST_CHECK_EQUAL(float_equal(0.0f, -0.0f), true);
    BOOST_CHECK_EQUAL(float_equal(23123.1233f, 23123.1233f), true);
    BOOST_CHECK_EQUAL(float_equal(-23123.1233f, -23123.1233f), true);
    BOOST_CHECK_EQUAL(float_equal(std::numeric_limits<float>::epsilon()/2.0f, 0.0f), true);
    BOOST_CHECK_EQUAL(float_equal(-std::numeric_limits<float>::epsilon()/2.0f, 0.0f), true);
    BOOST_CHECK_EQUAL(float_equal(std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::epsilon()), true);

    // false things
    BOOST_CHECK_EQUAL(float_equal(23123.1233f, -23123.1233f), false);
    BOOST_CHECK_EQUAL(float_equal(-23123.1233f, 23123.1233f), false);
    BOOST_CHECK_EQUAL(float_equal(1.0f, 0.0f), false);
    BOOST_CHECK_EQUAL(float_equal(1.0f, -1.0f), false);
    BOOST_CHECK_EQUAL(float_equal(std::numeric_limits<float>::epsilon(), 0.0f), false);
    BOOST_CHECK_EQUAL(float_equal(-std::numeric_limits<float>::epsilon(), 0.0f), false);
    BOOST_CHECK_EQUAL(float_equal(2*std::numeric_limits<float>::epsilon(), 0.0f), false);
    BOOST_CHECK_EQUAL(float_equal(-2*std::numeric_limits<float>::epsilon(), 0.0f), false);
    BOOST_CHECK_EQUAL(float_equal(std::numeric_limits<float>::epsilon(), -std::numeric_limits<float>::epsilon()), false);
}

BOOST_AUTO_TEST_CASE( test_float_gte ) {
    // true things
    BOOST_CHECK_EQUAL(float_gte(0.0f, 0.0f), true);
    BOOST_CHECK_EQUAL(float_gte(0.0f, -0.0f), true);
    BOOST_CHECK_EQUAL(float_gte(23123.1233f, 23123.1233f), true);
    BOOST_CHECK_EQUAL(float_gte(23123.1233f, -23123.1233f), true);
    BOOST_CHECK_EQUAL(float_gte(1.0f, 0.0f), true);
    BOOST_CHECK_EQUAL(float_gte(1.0f, -1.0f), true);
    BOOST_CHECK_EQUAL(float_gte(-20001.0f, -20001.0001f), true);
    BOOST_CHECK_EQUAL(float_gte(std::numeric_limits<float>::epsilon(), 0.0f), true);
    BOOST_CHECK_EQUAL(float_gte(-std::numeric_limits<float>::epsilon(), 0.0f), false);
    BOOST_CHECK_EQUAL(float_gte(std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::epsilon()), true);
    BOOST_CHECK_EQUAL(float_gte(std::numeric_limits<float>::epsilon(), -std::numeric_limits<float>::epsilon()), true);
    BOOST_CHECK_EQUAL(float_gte(2*std::numeric_limits<float>::epsilon(), 0.0f), true);
    BOOST_CHECK_EQUAL(float_gte(std::numeric_limits<float>::epsilon(), 0.0f), true);

    // false things
    BOOST_CHECK_EQUAL(float_gte(-1.0f, 1.0f), false);
    BOOST_CHECK_EQUAL(float_gte(-20001.2f, -20001.0001f), false);
    BOOST_CHECK_EQUAL(float_gte(-std::numeric_limits<float>::epsilon(), std::numeric_limits<float>::epsilon()), false);
    BOOST_CHECK_EQUAL(float_gte(-2*std::numeric_limits<float>::epsilon(), 0.0f), false);
    BOOST_CHECK_EQUAL(float_gte(-std::numeric_limits<float>::epsilon(), 0.0f), false);
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
