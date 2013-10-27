#include <tina/utils/bit_macros.h>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(BitMacrosTests)

BOOST_AUTO_TEST_CASE(test_BIT) {
  BOOST_CHECK_EQUAL(BIT(3), 0x8);
}

BOOST_AUTO_TEST_CASE(test_BIT_MASK) {
  BOOST_CHECK_EQUAL(BIT_MASK(3), 0x7);
}

BOOST_AUTO_TEST_CASE(test_BIT_RANGE_MASK) {
  BOOST_CHECK_EQUAL(BIT_RANGE_MASK(1, 3), 0xE);
}

BOOST_AUTO_TEST_CASE(test_BIT_RANGE) {
  BOOST_CHECK_EQUAL(BIT_RANGE(1, 3, 2), 0x4);
}

BOOST_AUTO_TEST_CASE(test_BIT_SET) {
  size_t x = 4;
  BOOST_CHECK_EQUAL(BIT_SET(x, 0), 0x5);
}

BOOST_AUTO_TEST_CASE(test_BIT_SET_RANGE) {
  size_t x = 4;
  BOOST_CHECK_EQUAL(BIT_SET_RANGE(x, 1, 3, 4), 0x8);
}

BOOST_AUTO_TEST_CASE(test_BIT_CLEAR) {
  size_t x = 4;
  BOOST_CHECK_EQUAL(BIT_CLEAR(x, 2), 0x0);
}

BOOST_AUTO_TEST_SUITE_END()
