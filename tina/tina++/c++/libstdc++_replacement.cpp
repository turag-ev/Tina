
#include <tina/debug/print.h>

extern "C" void __cxa_pure_virtual() { 
	while (1) {
		turag_error("C++ ERROR: __cxa_pure_virtual() called");
	}
} 
