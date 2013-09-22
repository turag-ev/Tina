#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <tina/array_buffer.h>

#include <ostream>

using namespace TURAG;

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_1 ) {
  ArrayBuffer<int, 20> test;
  
  test.push_back(12345);
  BOOST_CHECK_EQUAL(test[0], 12345);
  BOOST_CHECK_EQUAL(test.front(), 12345);
  
  test.push_back(6789);
  test.pop_front();
  BOOST_CHECK_EQUAL(test.front(), 6789);
}

template<typename Container>
void print_container_content(const Container& c) {
  int i = 0;
  for (const auto& entry : c) {
    std::cout << i++ << ": " << entry << std::endl;
  }
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_add_own_value ) {
  ArrayBuffer<int, 20> test;  
  test.push_back(12345);
  test.push_back(4242);
  test.push_back(6789);
  
  test.push_back(test[0]);  
  BOOST_CHECK_EQUAL(test.back(), 12345);
  
  test.push_back(test[1]);  
  BOOST_CHECK_EQUAL(test.back(), 4242);
  
  test.push_front(test[2]);
  BOOST_CHECK_EQUAL(test.front(), 6789);
  
  test.push_front(test[0]);
  BOOST_CHECK_EQUAL(test[0], 6789);
  BOOST_CHECK_EQUAL(test[1], 6789);
  
  test.insert(test.begin() + 2, test[3]);
  BOOST_CHECK_EQUAL(test[1], 6789);
  BOOST_CHECK_EQUAL(test[2], 4242);
  BOOST_CHECK_EQUAL(test[3], 12345);
  BOOST_CHECK_EQUAL(test[4], 4242);
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_erase ) {
  ArrayBuffer<int, 20> test;  
  test.push_back(12345);
  test.push_back(4242);
  test.push_back(6789);
  test.push_back(76357);
  test.push_back(34567);
  
  test.erase(test.begin() + 3);
  BOOST_CHECK_EQUAL(test.length(), 4);
  BOOST_CHECK_EQUAL(test[0], 12345);
  BOOST_CHECK_EQUAL(test[1], 4242);
  BOOST_CHECK_EQUAL(test[2], 6789);
  BOOST_CHECK_EQUAL(test[3], 34567);
  
  test.erase(test.begin() + 1, test.begin() + 3);
  BOOST_CHECK_EQUAL(test.length(), 2);
  BOOST_CHECK_EQUAL(test[0], 12345);
  BOOST_CHECK_EQUAL(test[1], 34567);
  
  test.erase(test.begin(), test.end()); 
  BOOST_CHECK_EQUAL(test.length(), 0);
}

//____________________________________________________________________________//
