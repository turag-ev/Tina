#include "binary.h"

#include <tina/utils/base64.h>


void _turag_binary_send(const char* header, const void* object, size_t size) {
	const size_t elen = turag_base64_encode_len(size);
	uint8_t paket[4+elen+3];

	// 4 header bytes
	paket[0] = header[0];
	paket[1] = header[1];
	paket[2] = header[2];
	paket[3] = header[3];

	// data bytes
	turag_base64_encode((uint8_t*)object, size, paket + 4);

	// end
	paket[elen] = TURAG_DEBUG_NEWLINE[0];
	paket[elen+1] = TURAG_DEBUG_NEWLINE[1];
	paket[elen+2] = '\0';

	turag_debug_puts((const char*)paket);
}
