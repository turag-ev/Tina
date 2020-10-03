#define TURAG_DEBUG_LOG_SOURCE TURAG_INTERNAL_LOG_SOURCE

#include "binary.h"

#ifdef TURAG_DEBUG_ENABLE_BINARY

#include <cstddef>
#include <cstdint>
#include <cstddef>
#include <tina/debug/print.h>
#include <tina++/utils/base64.h>
#include <tina++/algorithm.h>

using namespace TURAG;

extern "C"
void turag_binary_send_impl(char source, char object_id, const void* object, size_t size) {
	constexpr std::size_t newline_size = length(TURAG_DEBUG_NEWLINE) - 1;
	static_assert(newline_size == 0 || newline_size == 1 || newline_size == 2, "");

	const std::size_t elen = Base64::encodeLength(size);
	std::uint8_t paket[4 + elen + newline_size + 1];

	// 4 header bytes
	paket[0] = TURAG_DEBUG_LINE_PREFIX[0];
	paket[1] = source;
	paket[2] = TURAG_DEBUG_BINARY_PREFIX[0];
	paket[3] = object_id;

	// data bytes
	turag_base64_encode(static_cast<const std::uint8_t*>(object), size, paket+4);

	// end
	switch(newline_size) {
	case 0:
	    break;
	case 1:
	    paket[elen+4] = TURAG_DEBUG_NEWLINE[0];
	    break;
	case 2:
	    paket[elen+4] = TURAG_DEBUG_NEWLINE[0];
	    paket[elen+4+1] = TURAG_DEBUG_NEWLINE[1];
	    break;
	}
	paket[elen+4+newline_size] = '\0';

	turag_debug_puts_impl((const char*)paket);
}

#endif
