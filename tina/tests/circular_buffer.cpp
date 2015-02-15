#include "container_test.h"
#include <tina++/container/circular_buffer.h>

using namespace TURAG;

BOOST_AUTO_TEST_SUITE(CircularBufferTests)

#define CONTAINER CircularBuffer<Dummy, 20>
#define CONTAINER_NAME CircularBuffer

#include "container_test.inc"
#include "container_test_push_pop.inc"
#include "container_test_random_access.inc"

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
