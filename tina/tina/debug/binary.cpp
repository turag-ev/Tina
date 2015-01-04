#include "binary.h"

#include <cstddef>
#include <tina++/debug.h>
#include <tina++/utils/base64.h>

using namespace TURAG;

extern "C"
void _turag_binary_send(char source, char object_id, const void* object, size_t size) {
	const std::size_t elen = Base64::encodeLength(size);
	uint8_t paket[4+elen+3];

	// 4 header bytes
	paket[0] = TURAG_DEBUG_LINE_PREFIX[0];
	paket[1] = source;
	paket[2] = TURAG_DEBUG_BINARY_PREFIX[0];
	paket[3] = object_id;

	// data bytes
	turag_base64_encode(static_cast<const uint8_t*>(object), size, paket+4);

	// end
	paket[elen+4] = TURAG_DEBUG_NEWLINE[0];
	paket[elen+4+1] = TURAG_DEBUG_NEWLINE[1];
	paket[elen+4+2] = '\0';

	turag_debug_puts((const char*)paket);
}
