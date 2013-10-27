#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <tina/array_buffer.h>
#include <tina/algorithm.h>

#include <ostream>
#include <deque>

using namespace TURAG;

BOOST_AUTO_TEST_SUITE(ArrayBufferTests)

template<typename Container>
void print_container_content(const Container& c) {
  int i = 0;
  for (const auto& entry : c) {
    std::cout << i++ << ": " << entry << std::endl;
  }
}

class Dummy {
public:
  Dummy() {
    ++instances;
  }

  Dummy(int i) :
    i_(i)
  {
    ++instances;
  }

  ~Dummy() {
    --instances;
  }

  Dummy(const Dummy& other) :
    i_(other.i_)
  { ++instances; }

  Dummy(const Dummy&& other) :
    i_(other.i_)
  { ++instances; }

  Dummy& operator = (const Dummy&& other) {
    i_ = other.i_;
    return *this;
  }

  bool operator==(int i) const {
    return i == i_;
  }

  bool operator!=(int i) const {
    return i != i_;
  }

  bool operator==(const Dummy& other) const {
    return i_ == other.i_;
  }

  bool operator!=(const Dummy& other) const {
    return i_ != other.i_;
  }

  static std::size_t instances;

private:
  int i_;

  friend std::ostream& operator<<(std::ostream& str, const Dummy& dummy);
};

std::ostream& operator<<(std::ostream& str, const Dummy& dummy) {
  return str << dummy.i_;
}

std::size_t Dummy::instances = 0;

template<typename Container>
boost::test_tools::predicate_result
check_container_content(const Container& a, std::initializer_list<Dummy> b) {
  if (length(a) != length(b)) {
    boost::test_tools::predicate_result res(false);
    res.message() << "Different container sizes [ " << length(a) << " != " << length(b) << " ]";
    return res;
  }

  typename Container::const_iterator aiter = a.cbegin();
  std::initializer_list<Dummy>::const_iterator biter = b.begin();
  std::size_t i = 0;
  for (; biter != b.end(); ++aiter, ++biter, ++i) {
    if (*aiter != *biter) {
      boost::test_tools::predicate_result res(false);
      res.message() << "Different container content at item " << i << " [ " << *aiter << " != " << *biter << " ]";
      return res;
    }
  }

  return true;
}

#define CONTAINER ArrayBuffer<Dummy, 20>
//#define CONTAINER std::deque<Dummy>

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_push_front ) {
  {
    CONTAINER test;

    test.push_front(1);
    test.push_front(2);
    test.push_front(3);
    test.push_front(4);
    test.push_front(5);
    test.push_front(6);
    test.push_front(7);

    BOOST_CHECK(check_container_content(test, {7, 6, 5, 4, 3, 2, 1}));

    test.push_back(0);
    test.push_front(8);
    test.push_front(9);

    BOOST_CHECK(check_container_content(test, {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));

    BOOST_CHECK_EQUAL(Dummy::instances, 10);
    Dummy::instances = 10;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_emplace_front ) {
  {
    CONTAINER test;

    test.emplace_front(1);
    test.emplace_front(2);
    test.emplace_front(3);
    test.emplace_front(4);
    test.emplace_front(5);
    test.emplace_front(6);
    test.emplace_front(7);

    BOOST_CHECK(check_container_content(test, {7, 6, 5, 4, 3, 2, 1}));

    test.emplace_back(0);
    test.emplace_front(8);
    test.emplace_front(9);

    BOOST_CHECK(check_container_content(test, {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));

    BOOST_CHECK_EQUAL(Dummy::instances, 10);
    Dummy::instances = 10;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_push_back ) {
  {
    CONTAINER test;

    test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    test.push_back(4);
    test.push_back(5);
    test.push_back(6);
    test.push_back(7);

    BOOST_CHECK(check_container_content(test, {1, 2, 3, 4, 5, 6, 7}));

    BOOST_CHECK_EQUAL(test.front(), 1);
    BOOST_CHECK_EQUAL(test[0], 1);
    BOOST_CHECK_EQUAL(*test.begin(), 1);
    BOOST_CHECK_EQUAL(*test.cbegin(), 1);

    BOOST_CHECK_EQUAL(test.back(), 7);
    BOOST_CHECK_EQUAL(test[6], 7);

    BOOST_CHECK_EQUAL(test.size(), 7);
//    BOOST_CHECK_EQUAL(test.capacity(), 20);

    BOOST_CHECK_EQUAL(Dummy::instances, 7);
    Dummy::instances = 7;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_emplace_back ) {
  {
    CONTAINER test;

    test.emplace_back(1);
    test.emplace_back(2);
    test.emplace_back(3);
    test.emplace_back(4);
    test.emplace_back(5);
    test.emplace_back(6);
    test.emplace_back(7);

    BOOST_CHECK(check_container_content(test, {1, 2, 3, 4, 5, 6, 7}));

    BOOST_CHECK_EQUAL(test.front(), 1);
    BOOST_CHECK_EQUAL(test[0], 1);
    BOOST_CHECK_EQUAL(*test.begin(), 1);
    BOOST_CHECK_EQUAL(*test.cbegin(), 1);

    BOOST_CHECK_EQUAL(test.back(), 7);
    BOOST_CHECK_EQUAL(test[6], 7);

    BOOST_CHECK_EQUAL(test.size(), 7);
//    BOOST_CHECK_EQUAL(test.capacity(), 20);

    BOOST_CHECK_EQUAL(Dummy::instances, 7);
    Dummy::instances = 7;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_add_own_value ) {
  {
    CONTAINER test;
    test.push_back(1);
    test.push_back(2);
    test.push_back(3);

    test.push_back(test[0]);
    BOOST_CHECK(check_container_content(test, {1, 2, 3, 1}));

    test.push_back(test[1]);
    BOOST_CHECK(check_container_content(test, {1, 2, 3, 1, 2}));

    test.push_front(test[2]);
    BOOST_CHECK(check_container_content(test, {3, 1, 2, 3, 1, 2}));

    test.push_front(test[0]);
    BOOST_CHECK(check_container_content(test, {3, 3, 1, 2, 3, 1, 2}));

    test.insert(test.begin() + 2, test[2]);
    BOOST_CHECK(check_container_content(test, {3, 3, 1, 1, 2, 3, 1, 2}));

    BOOST_CHECK_EQUAL(Dummy::instances, 8);
    Dummy::instances = 8;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_erase ) {
  CONTAINER test;
  test.push_back(1);
  test.push_back(2);
  test.push_back(3);
  test.push_back(4);
  test.push_back(5);

  test.erase(test.begin() + 3);
  BOOST_CHECK_EQUAL(test.size(), 4);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 5}));

  test.erase(test.begin() + 1, test.begin() + 3);
  BOOST_CHECK_EQUAL(test.size(), 2);
  BOOST_CHECK(check_container_content(test, {1, 5}));

  test.erase(test.begin(), test.end());
  BOOST_CHECK_EQUAL(test.size(), 0);

  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_insert ) {
  CONTAINER test;
  test.push_back(1);
  test.push_back(2);
  test.push_back(3);
  test.push_back(4);
  test.push_back(5);

  test.insert(test.begin() + 3, 6);
  BOOST_CHECK_EQUAL(test.size(), 6);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 6, 4, 5}));

  test.insert(test.begin() + 6, 7);
  BOOST_CHECK_EQUAL(test.size(), 7);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 6, 4, 5, 7}));

  test.insert(test.begin(), 0);
  BOOST_CHECK_EQUAL(test.size(), 8);
  BOOST_CHECK(check_container_content(test, {0, 1, 2, 3, 6, 4, 5, 7}));
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_emplace ) {
  CONTAINER test;
  test.push_back(1);
  test.push_back(2);
  test.push_back(3);
  test.push_back(4);
  test.push_back(5);

  test.emplace(test.begin() + 3, 6);
  BOOST_CHECK_EQUAL(test.size(), 6);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 6, 4, 5}));

  test.emplace(test.begin() + 6, 7);
  BOOST_CHECK_EQUAL(test.size(), 7);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 6, 4, 5, 7}));

  test.emplace(test.begin(), 0);
  BOOST_CHECK_EQUAL(test.size(), 8);
  BOOST_CHECK(check_container_content(test, {0, 1, 2, 3, 6, 4, 5, 7}));
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_pop_front ) {
  CONTAINER test;
  test.push_back(1);
  test.push_back(2);
  test.push_back(3);
  test.push_back(4);
  test.push_back(5);

  test.pop_front();
  BOOST_CHECK_EQUAL(test.size(), 4);
  BOOST_CHECK(check_container_content(test, {2, 3, 4, 5}));

  BOOST_CHECK_EQUAL(Dummy::instances, 4);
  Dummy::instances = 4;
}

BOOST_AUTO_TEST_CASE( test_ArrayBuffer_pop_back ) {
  CONTAINER test;
  test.push_back(1);
  test.push_back(2);
  test.push_back(3);
  test.push_back(4);
  test.push_back(5);

  test.pop_back();
  BOOST_CHECK_EQUAL(test.size(), 4);
  BOOST_CHECK(check_container_content(test, {1, 2, 3, 4}));

  BOOST_CHECK_EQUAL(Dummy::instances, 4);
  Dummy::instances = 4;
}

/*
BOOST_AUTO_TEST_CASE( test_ArrayBuffer_copy ) {
  {
    CONTAINER test1;
    test1.push_back(1);
    test1.push_back(2);
    test1.push_back(3);
    test1.push_back(4);
    test1.push_back(5);

    CONTAINER test2 = test1;

    BOOST_CHECK_EQUAL(test1.size(), 5);
    BOOST_CHECK(check_container_content(test1, {1, 2, 3, 4, 5}));

    BOOST_CHECK_EQUAL(test2.size(), 5);
    BOOST_CHECK(check_container_content(test2, {1, 2, 3, 4, 5}));

    BOOST_CHECK_EQUAL(Dummy::instances, 10);
    Dummy::instances = 10;
  }
  BOOST_CHECK_EQUAL(Dummy::instances, 0);
  Dummy::instances = 0;
}*/

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
