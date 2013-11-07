#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <tina++/geometry/units.h>

using namespace TURAG;
using namespace TURAG::Units;

BOOST_AUTO_TEST_SUITE(UnitsTests)

BOOST_AUTO_TEST_CASE( test_Unit_toInt ) {
  Length s2 = 700 * mm;

  BOOST_CHECK_EQUAL(s2.toInt(m), 1);
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
