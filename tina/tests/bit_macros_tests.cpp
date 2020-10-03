#include <tina/utils/bit_macros.h>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(BitMacrosTests)

BOOST_AUTO_TEST_CASE(test_turag_bit) {
  BOOST_CHECK_EQUAL(turag_bit(3), 0x8);
}

BOOST_AUTO_TEST_CASE(test_turag_bit_mask) {
  BOOST_CHECK_EQUAL(turag_bit_mask(3), 0x7);
}

BOOST_AUTO_TEST_CASE(test_turag_bit_range_mask) {
  BOOST_CHECK_EQUAL(turag_bit_range_mask(1, 3), 0xE);
}

BOOST_AUTO_TEST_CASE(test_turag_bit_range) {
  BOOST_CHECK_EQUAL(turag_bit_range(1, 3, 2), 0x4);
}

BOOST_AUTO_TEST_CASE(test_turag_set_bit) {
  size_t x = 4;
  BOOST_CHECK_EQUAL(turag_set_bit(x, 0), 0x5);
}

BOOST_AUTO_TEST_CASE(test_turag_set_bit_range) {
  size_t x = 4;
  BOOST_CHECK_EQUAL(turag_set_bit_range(x, 1, 3, 4), 0x8);
}

BOOST_AUTO_TEST_CASE(test_turag_clear_bit) {
  size_t x = 4;
  BOOST_CHECK_EQUAL(turag_clear_bit(x, 2), 0x0);
}

BOOST_AUTO_TEST_SUITE_END()
