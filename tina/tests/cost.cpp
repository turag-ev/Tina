#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <limits>

#include <tina++/utils/cost.h>

using namespace TURAG;

BOOST_AUTO_TEST_SUITE(CostsTests)

BOOST_AUTO_TEST_CASE( test_Costs ) {
  Cost c(0);
  BOOST_CHECK_EQUAL(c.toUnsigned(), 0);

  c -= 500U;
  BOOST_CHECK_EQUAL(c.toUnsigned(), 0);

  c += 1000U;
  BOOST_CHECK_EQUAL(c.toUnsigned(), 1000);

  c += -500;
  BOOST_CHECK_EQUAL(c.toUnsigned(), 500);

  c += int(10000);
  BOOST_CHECK_EQUAL(c.toUnsigned(), 10500);

  c -= int(1000);
  BOOST_CHECK_EQUAL(c.toUnsigned(), 9500);

  c += int(-1000);
  BOOST_CHECK_EQUAL(c.toUnsigned(), 8500);

  c -= int(-1000);
  BOOST_CHECK_EQUAL(c.toUnsigned(), 9500);

	c += Cost(100) + Cost(100);
  BOOST_CHECK_EQUAL(c.toUnsigned(), 9700);
}

BOOST_AUTO_TEST_CASE( test_Cost_Max ) {
  Cost c(1);

  c += COST_MAX;
  BOOST_CHECK_EQUAL(c.toUnsigned(), COST_MAX.toUnsigned());

  c += 1;
  BOOST_CHECK_EQUAL(c.toUnsigned(), COST_MAX.toUnsigned());

  c += 1;
  BOOST_CHECK_EQUAL(c.toUnsigned(), COST_MAX.toUnsigned());

  c -= 2;
  BOOST_CHECK_EQUAL(c.toUnsigned(), COST_MAX.toUnsigned()-2);

  c += 2;
  BOOST_CHECK_EQUAL(c.toUnsigned(), COST_MAX.toUnsigned());

  c -= COST_MAX;
  BOOST_CHECK_EQUAL(c.toUnsigned(), 0);

  c += COST_MAX;
  c -= COST_INFINITY;
  BOOST_CHECK_EQUAL(c.toUnsigned(), 0);

  c -= std::numeric_limits<int>::min();
  BOOST_CHECK_EQUAL(c.toUnsigned(), -std::numeric_limits<int>::min());

	BOOST_CHECK_EQUAL((COST_MAX + COST_MAX).toUnsigned(), COST_MAX.toUnsigned());
  BOOST_CHECK_EQUAL((COST_MAX + Cost(1)).toUnsigned(), COST_MAX.toUnsigned());
	BOOST_CHECK_EQUAL((Cost(1) + COST_MAX).toUnsigned(), COST_MAX.toUnsigned());
}

BOOST_AUTO_TEST_CASE( test_Cost_Inf ) {
  Cost c = COST_INFINITY;
  BOOST_CHECK_EQUAL(c.toUnsigned(), COST_INFINITY.toUnsigned());

  c += 500;
  BOOST_CHECK_EQUAL(c.toUnsigned(), COST_INFINITY.toUnsigned());

  c -= 1000;
  BOOST_CHECK_EQUAL(c.toUnsigned(), COST_INFINITY.toUnsigned());

  Cost c2(0);
  c2 += COST_INFINITY;
  BOOST_CHECK_EQUAL(c2.toUnsigned(), COST_INFINITY.toUnsigned());

  c2 -= COST_MAX;
  BOOST_CHECK_EQUAL(c2.toUnsigned(), COST_INFINITY.toUnsigned());

  c2 -= COST_INFINITY;
  BOOST_CHECK_EQUAL(c2.toUnsigned(), COST_INFINITY.toUnsigned());

  Cost c3(COST_MAX);
  c3 += COST_INFINITY;
  BOOST_CHECK_EQUAL(c2.toUnsigned(), COST_INFINITY.toUnsigned());
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
