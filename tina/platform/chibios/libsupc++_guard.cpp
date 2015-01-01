/**
 * \file platform/chibios/libsupc++_guard.cpp
 * \author Martin Oemus <martin@oemus.net>
 * \date Oktober 2014
 *
 * \brief C++ runtime-support.
 * 
 * Diese Datei stellt C++-Laufzeitunterstützung für die Verwendung
 * thread-sicherer funktions-statischer Klasseninstanzen bereit.
 * 
 * Dies ermöglicht zum Beispiel die Implementierung von Singletons
 * ohne die Benutzung globaler Instanzen, wenn new nicht verfügbar ist.
 * 
 * \note Die vorliegende Implementierung wurde aus der zur gcc 4.7
 * gehörenden libsupc++-Bibliothek entnommen und etwas vereinfacht.
 * Dadurch kann diese Version ausschließlich auf der ARM-Platform 
 * (ab armv6) mit ChibiOS als Betriebssystem benutzt werden.
 * Die Futex-Syscalls wurden durch die Benutzung einer Semaphore 
 * ersetzt. Da der Quellcode ansonsten kaum verändert wurde,
 * sollten die Anpassungen leicht nachvollziehbar sein.
 */


#include <tina/tina.h>

#if TURAG_USE_TINA_CPP_RUNTIME_SUPPORT

#include <ch.h>


// Copyright (C) 2002, 2004, 2006, 2008, 2009, 2010, 2011, 2012, 2014
// Free Software Foundation, Inc.
//  
// This file is part of GCC.
//
// GCC is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.

// GCC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

// Written by Mark Mitchell, CodeSourcery LLC, <mark@codesourcery.com>
// Thread support written by Jason Merrill, Red Hat Inc. <jason@redhat.com>



namespace __cxxabiv1
{
extern "C" {

  // The ARM EABI uses the least significant bit of a 32-bit
  // guard variable.  */
#define _GLIBCXX_GUARD_TEST(x) ((*(x) & 1) != 0)
#define _GLIBCXX_GUARD_SET(x) *(x) = 1
#define _GLIBCXX_GUARD_BIT 1
#define _GLIBCXX_GUARD_PENDING_BIT __guard_test_bit (1, 1)
#define _GLIBCXX_GUARD_WAITING_BIT __guard_test_bit (2, 1)
  typedef int __guard;

  // We also want the element size in array cookies.
#define _GLIBCXX_ELTSIZE_IN_COOKIE 1
    
}
} // namespace __cxxabiv1




// Even if the CPU doesn't need a memory barrier, we need to ensure
// that the compiler doesn't reorder memory accesses across the
// barriers.
#ifndef _GLIBCXX_READ_MEM_BARRIER
#define _GLIBCXX_READ_MEM_BARRIER __asm __volatile ("":::"memory")
#endif
#ifndef _GLIBCXX_WRITE_MEM_BARRIER
#define _GLIBCXX_WRITE_MEM_BARRIER __asm __volatile ("":::"memory")
#endif



// Thread-safe static local initialization support.

# ifndef _GLIBCXX_GUARD_TEST_AND_ACQUIRE
inline bool
__test_and_acquire (__cxxabiv1::__guard *g)
{
  bool b = _GLIBCXX_GUARD_TEST (g);
  _GLIBCXX_READ_MEM_BARRIER;
  return b;
}
#  define _GLIBCXX_GUARD_TEST_AND_ACQUIRE(G) __test_and_acquire (G)
# endif



//
// Here are C++ run-time routines for guarded initialization of static
// variables. There are 4 scenarios under which these routines are called:
//
//   1. Threads not supported (__GTHREADS not defined)
//   2. Threads are supported but not enabled at run-time.
//   3. Threads enabled at run-time but __gthreads_* are not fully POSIX.
//   4. Threads enabled at run-time and __gthreads_* support all POSIX threads
//      primitives we need here.
//
// The old code supported scenarios 1-3 but was broken since it used a global
// mutex for all threads and had the mutex locked during the whole duration of
// initialization of a guarded static variable. The following created a
// dead-lock with the old code.
//
//	Thread 1 acquires the global mutex.
//	Thread 1 starts initializing static variable.
//	Thread 1 creates thread 2 during initialization.
//	Thread 2 attempts to acquire mutex to initialize another variable.
//	Thread 2 blocks since thread 1 is locking the mutex.
//	Thread 1 waits for result from thread 2 and also blocks. A deadlock.
//
// The new code here can handle this situation and thus is more robust. However,
// we need to use the POSIX thread condition variable, which is not supported
// in all platforms, notably older versions of Microsoft Windows. The gthr*.h
// headers define a symbol __GTHREAD_HAS_COND for platforms that support POSIX
// like condition variables. For platforms that do not support condition
// variables, we need to fall back to the old code.

// If _GLIBCXX_USE_FUTEX, no global mutex or condition variable is used,
// only atomic operations are used together with futex syscall.
// Valid values of the first integer in guard are:
// 0				  No thread encountered the guarded init
//				  yet or it has been aborted.
// _GLIBCXX_GUARD_BIT		  The guarded static var has been successfully
//				  initialized.
// _GLIBCXX_GUARD_PENDING_BIT	  The guarded static var is being initialized
//				  and no other thread is waiting for its
//				  initialization.
// (_GLIBCXX_GUARD_PENDING_BIT    The guarded static var is being initialized
//  | _GLIBCXX_GUARD_WAITING_BIT) and some other threads are waiting until
//				  it is initialized.

namespace __cxxabiv1 {
namespace {

static inline int __guard_test_bit (const int __byte, const int __val) {
	union { int __i; char __c[sizeof (int)]; } __u = { 0 };
	__u.__c[__byte] = __val;
	return __u.__i;
}
	
// static initialization of our semaphore
SEMAPHORE_DECL(semaphore, 0);


} // namespace



extern "C"
int __cxa_guard_acquire (__guard *g) {
	// If the target can reorder loads, we need to insert a read memory
	// barrier so that accesses to the guarded variable happen after the
	// guard test.
	if (_GLIBCXX_GUARD_TEST_AND_ACQUIRE (g)) {
		return 0;
	}

	int *gi = (int *) (void *) g;
	const int guard_bit = _GLIBCXX_GUARD_BIT;
	const int pending_bit = _GLIBCXX_GUARD_PENDING_BIT;
	const int waiting_bit = _GLIBCXX_GUARD_WAITING_BIT;

	while (1) {
		int expected(0);
		if (__atomic_compare_exchange_n(gi, &expected, pending_bit, false,
						__ATOMIC_ACQ_REL,
						__ATOMIC_ACQUIRE)) {
			// This thread should do the initialization.
			return 1;
		}
			
		if (expected == guard_bit) {
			// Already initialized.
			return 0;	
		}

		if (expected == pending_bit) {
			// Use acquire here.
			int newv = expected | waiting_bit;
			if (!__atomic_compare_exchange_n(gi, &expected, newv, false,
							__ATOMIC_ACQ_REL, 
							__ATOMIC_ACQUIRE)) {
				if (expected == guard_bit) {
					// Make a thread that failed to set the
					// waiting bit exit the function earlier,
					// if it detects that another thread has
					// successfully finished initialising.
					return 0;
				}
				
				if (expected == 0) {
					continue;
				}
			}
			
			expected = newv;
		}

		//syscall (SYS_futex, gi, _GLIBCXX_FUTEX_WAIT, expected, 0);
		chSemWait(&semaphore);
	}
}


extern "C"
void __cxa_guard_abort (__guard *) {
	// This function is called if the initialization terminates by throwing an exception. 
	// We don't support exceptions, hence we don't need it.
}

extern "C"
void __cxa_guard_release (__guard *g) {
	int *gi = (int *) (void *) g;
	const int guard_bit = _GLIBCXX_GUARD_BIT;
	const int waiting_bit = _GLIBCXX_GUARD_WAITING_BIT;
	int old = __atomic_exchange_n (gi, guard_bit, __ATOMIC_ACQ_REL);

	if ((old & waiting_bit) != 0) {
		//syscall (SYS_futex, gi, _GLIBCXX_FUTEX_WAKE, INT_MAX);
		chSemReset(&semaphore, 0);
	}
}
  
} // namespace __cxxabiv1


#endif // TURAG_USE_TINA_CPP_RUNTIME_SUPPORT
