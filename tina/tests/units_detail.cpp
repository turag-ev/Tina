#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <tina/geometry/units.h>

using namespace TURAG::Units;
using namespace TURAG::Units::detail;

BOOST_AUTO_TEST_CASE( gcd_func ) {
  BOOST_CHECK_EQUAL(gcd(15, 18), 3);
  BOOST_CHECK_EQUAL(gcd(9, 18), 9);
}

BOOST_AUTO_TEST_CASE( rational_mul_func ) {
  typedef Rational<6,1> Six;
  typedef Rational<1,6> Sixth; 
  typedef Rational<2,3> d2_3; 
  
  typedef typename rational_mul<Sixth, 6>::type Test1;
  BOOST_CHECK_EQUAL(Test1::z(), 1);
  BOOST_CHECK_EQUAL(Test1::n(), 1);
 
  typedef typename rational_mul<d2_3, 2>::type Test2;
  BOOST_CHECK_EQUAL(Test2::z(), 4);
  BOOST_CHECK_EQUAL(Test2::n(), 3);
  
  typedef typename rational_mul<d2_3, 3>::type Test3;
  BOOST_CHECK_EQUAL(Test3::z(), 2);
  BOOST_CHECK_EQUAL(Test3::n(), 1);
}

BOOST_AUTO_TEST_CASE( rational_div_func ) {
  typedef Rational<6,1> Six;
  typedef Rational<1,6> Sixth; 
  typedef Rational<2,3> d2_3; 
  
  typedef typename rational_div<Six, 6>::type Test1;
  BOOST_CHECK_EQUAL(Test1::z(), 1);
  BOOST_CHECK_EQUAL(Test1::n(), 1); 
  
  typedef typename rational_div<Sixth, 6>::type Test2;
  BOOST_CHECK_EQUAL(Test2::z(), 1);
  BOOST_CHECK_EQUAL(Test2::n(), 36); 
  
  typedef typename rational_div<d2_3, 6>::type Test3;
  BOOST_CHECK_EQUAL(Test3::z(), 1);
  BOOST_CHECK_EQUAL(Test3::n(), 9); 
  
  typedef typename rational_div<Six, 1>::type Test4;
  BOOST_CHECK_EQUAL(Test4::z(), 6);
  BOOST_CHECK_EQUAL(Test4::n(), 1); 
  
  typedef typename rational_div<Null, 6>::type Test5;
  BOOST_CHECK_EQUAL(Test5::z(), 0);
  BOOST_CHECK_EQUAL(Test5::n(), 1); 
}

BOOST_AUTO_TEST_CASE( rational_add_func ) {
  typedef Rational<6,1> Six;
  typedef Rational<1,6> Sixth; 
  typedef Rational<2,3> d2_3; 
  
  typedef typename rational_add<Six, Sixth>::type Test1;
  BOOST_CHECK_EQUAL(Test1::z(), 37);
  BOOST_CHECK_EQUAL(Test1::n(), 6); 
  
  typedef typename rational_add<Sixth, d2_3>::type Test2;
  BOOST_CHECK_EQUAL(Test2::z(), 5);
  BOOST_CHECK_EQUAL(Test2::n(), 6); 
  
  typedef typename rational_add<d2_3, d2_3>::type Test3;
  BOOST_CHECK_EQUAL(Test3::z(), 4);
  BOOST_CHECK_EQUAL(Test3::n(), 3);
}

BOOST_AUTO_TEST_CASE( rational_sub_func ) {
  typedef Rational<6,1> Six;
  typedef Rational<1,6> Sixth; 
  typedef Rational<2,3> d2_3; 
  
  typedef typename rational_sub<Six, Sixth>::type Test1;
  BOOST_CHECK_EQUAL(Test1::z(), 35);
  BOOST_CHECK_EQUAL(Test1::n(), 6); 
  
  typedef typename rational_sub<Sixth, d2_3>::type Test2;
  BOOST_CHECK_EQUAL(Test2::z(), -1);
  BOOST_CHECK_EQUAL(Test2::n(), 2); 
  
  typedef typename rational_sub<d2_3, d2_3>::type Test3;
  BOOST_CHECK_EQUAL(Test3::z(), 0);
  BOOST_CHECK_EQUAL(Test3::n(), 1); 
}

//____________________________________________________________________________//
