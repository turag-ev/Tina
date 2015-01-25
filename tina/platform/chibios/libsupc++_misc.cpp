/**
 * \file platform/chibios/libsupc++_misc.cpp
 * \author Martin Oemus <martin@oemus.net>
 * \date April 2014
 *
 * \brief C++ runtime-support.
 *
 * Diese Datei stellt die C++-Laufzeitunterstützung zur Verwendung
 * abstrakter Klassenfunktionen bereit.
 *
 */

#include <tina/tina.h>

#if TURAG_USE_TINA_CPP_RUNTIME_SUPPORT


#include <tina/debug/print.h>


namespace __cxxabiv1
{

extern "C" void __cxa_pure_virtual(void) __attribute__ ((__noreturn__));

extern "C" void __cxa_pure_virtual() {
	while (1) {
		turag_error("C++ ERROR: __cxa_pure_virtual() called");
	}
}

} // namespace __cxxabiv1


#endif // TURAG_USE_TINA_CPP_RUNTIME_SUPPORT
