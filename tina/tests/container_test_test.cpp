#include "container_test.h"
#include <deque>

using namespace TURAG;

BOOST_AUTO_TEST_SUITE(ContainerTestTests)

#define CONTAINER std::deque<Dummy>
#define CONTAINER_NAME Deque

#include "container_test.inc"
#include "container_test_push_pop.inc"
#include "container_test_insert.inc"
#include "container_test_random_access.inc"

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
