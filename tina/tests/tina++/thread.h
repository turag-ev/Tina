/**
 * \file thread.h
 * \author Richard Liebscher <richard.liebscher@googlemail.com>
 * \date 2011,2012
 *
 * \brief implementation of thread, mutex and scoped_lock
 *
 */

#ifndef TINAPP_SIM_THREAD_H
#define TINAPP_SIM_THREAD_H

#include <tina++/tina.h>
#include "time.h"

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// Thread

template<size_t size = 0>
class Thread {
protected:
  char stack_[size];
};

////////////////////////////////////////////////////////////////////////////////
// Mutex

class Mutex {};

////////////////////////////////////////////////////////////////////////////////
// ConditionVariable

class ConditionVariable {};

} // namespace TURAG

#endif // TINAPP_SIM_THREAD_H
