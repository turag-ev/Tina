#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <tina/helper/macros.h>

BOOST_AUTO_TEST_SUITE(MacrosTests)

#define SUM0() (0)
#define SUM1(x) (x)
#define SUM2(x,y) ((x) + (y))
#define SUM3(x,y,z) ((x) + (y) + (z))

static int sum0() { return 0; }
static int sum1(int x) { return x; }
static int sum2(int x, int y) { return x + y; }
static int sum3(int x, int y, int z) { return x + y + z; }

BOOST_AUTO_TEST_CASE( test_NARG ) {
  // fails: BOOST_CHECK_EQUAL(TURAG_INTERNAL_NARG_(), 0);
  BOOST_CHECK_EQUAL(TURAG_INTERNAL_NARG_(a), 1);
  BOOST_CHECK_EQUAL(TURAG_INTERNAL_NARG_(a, b), 2);

#define SUM(...) TURAG_CONCAT(SUM, TURAG_INTERNAL_NARG_(__VA_ARGS__)) (__VA_ARGS__)
  BOOST_CHECK_EQUAL(SUM(1,2,3), 6);

  int x = 3;
  BOOST_CHECK_EQUAL(SUM(1,2,x), 6);
  BOOST_CHECK_EQUAL(SUM(x,2,1), 6);

  //BOOST_CHECK_EQUAL(SUM(), 0);
#undef SUM
}

BOOST_AUTO_TEST_CASE( test_NARG2 ) {
#define SUM(...) TURAG_CONCAT(sum, TURAG_INTERNAL_NARG_(__VA_ARGS__)) (__VA_ARGS__)
  BOOST_CHECK_EQUAL(SUM(1,2,3), 6);

  int x = 3;
  BOOST_CHECK_EQUAL(SUM(1,2,x), 6);
  BOOST_CHECK_EQUAL(SUM(x,2,1), 6);

  //BOOST_CHECK_EQUAL(SUM(), 0);
#undef SUM
}

BOOST_AUTO_TEST_CASE( test_VFUNC ) {
#define SUM(...) TURAG_VFUNC(SUM, __VA_ARGS__)
  BOOST_CHECK_EQUAL(SUM(1,2,3), 6);

  int x = 3;
  BOOST_CHECK_EQUAL(SUM(1,2,x), 6);
  BOOST_CHECK_EQUAL(SUM(x,2,1), 6);
  
  //BOOST_CHECK_EQUAL(SUM(), 0);
#undef SUM
}

BOOST_AUTO_TEST_CASE( test_VFUNC2 ) {
#define SUM(...) TURAG_VFUNC(sum, __VA_ARGS__)
  BOOST_CHECK_EQUAL(SUM(1,2,3), 6);

  int x = 3;
  BOOST_CHECK_EQUAL(SUM(1,2,x), 6);
  BOOST_CHECK_EQUAL(SUM(x,2,1), 6);
  
  BOOST_CHECK_EQUAL(SUM(1), 1);
  //BOOST_CHECK_EQUAL(SUM(), 0);
#undef SUM
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
