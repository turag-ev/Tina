#ifndef TESTS_CONTAINER_TEST_H
#define TESTS_CONTAINER_TEST_H

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <tina++/algorithm.h>
#include <tina/helper/macros.h>

#include <iostream>

namespace TURAG {

#define TURAG_CONTAINER_TEST(test) TURAG_CONCAT4(ContainerTest_, CONTAINER_NAME, _, test)

template<typename Container>
void print_container_content(const Container& c) {
  int i = 0;
  for (auto&& entry : c) {
    std::cout << i++ << ": " << entry << std::endl;
  }
}

class Dummy {
public:
  Dummy() :
    i_(0)
  {
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

  Dummy(Dummy&& other) :
    i_(other.i_)
  { ++instances; }

  Dummy& operator = (const Dummy& other) {
    i_ = other.i_;
    return *this;
  }

  Dummy& operator = (Dummy&& other) {
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

inline std::ostream& operator<<(std::ostream& str, const Dummy& dummy) {
  return str << dummy.i_;
}


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

} // namespace TURAG

#endif // TESTS_CONTAINER_TEST_H
