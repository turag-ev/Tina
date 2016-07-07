#include "container_test.h"
#include <tina++/container/array_buffer.h>

using namespace TURAG;

BOOST_AUTO_TEST_SUITE(ArrayBufferTests)

#define CONTAINER ArrayBuffer<Dummy, 20>
#define CONTAINER_NAME ArrayBuffer

#include "container_test.inc"
#include "container_test_push_pop.inc"
#include "container_test_insert.inc"
#include "container_test_random_access.inc"

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//
