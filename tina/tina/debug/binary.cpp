#incude "binary.h"

#include <tina++/utils/base64.h>

using namespace TURAG;

extern "C"
void _turag_binary_send(const char* header, const void* object, size_t size) {
	const std:size_t elen = Base64::encodeLength(size);
	uint8_t paket[4+elen+3];

	// 4 header bytes
	encoded[0] = header[0];
	encoded[1] = header[1];
	encoded[2] = header[2];
	encoded[3] = header[3];

	// data bytes
	turag_base64_encode(object, size, encoded);

	// end
	encoded[elen] = TURAG_DEBUG_NEWLINE[0];
	encoded[elen+1] = TURAG_DEBUG_NEWLINE[1];
	encoded[elen+2] = '\0';

	turag_debug_puts((const char*)paket);
}
