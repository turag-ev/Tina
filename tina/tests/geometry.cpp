#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <tina++/geometry.h>

using namespace TURAG;
using namespace TURAG::Units;

BOOST_AUTO_TEST_SUITE(GeometryTests)

BOOST_AUTO_TEST_CASE( test_Rect_in_range ) {
  Rect rect(Point(0*m,0*m), 1*m, 1*m);

  BOOST_CHECK_EQUAL(true, in_range(Point(null, null), rect, null));
  BOOST_CHECK_EQUAL(true, in_range(Point(500*mm, 500*mm), rect, 50*mm));
  BOOST_CHECK_EQUAL(true, in_range(Point(-500*mm, 0*mm), rect, 510*mm));
  BOOST_CHECK_EQUAL(true, in_range(Point(1.5*mm, 0*mm), rect, 510*mm));
  BOOST_CHECK_EQUAL(true, in_range(Point(1*mm, 0.5*mm), rect, 510*mm));
  
  BOOST_CHECK_EQUAL(false, in_range(Point(-500*mm, 0*mm), rect, 490*mm));
  BOOST_CHECK_EQUAL(false, in_range(Point(-500*mm, 1500*mm), rect, 510*mm));
}

BOOST_AUTO_TEST_SUITE_END()
