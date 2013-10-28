#ifndef TURAG_BYTES_H
#define TURAG_BYTES_H

#include <tina/types.h>
#include <tina/tina.h>

_always_inline uint8_t highbyte(uint16_t halfword) {
	return (halfword >> 8) & 0xFF;
}

_always_inline uint8_t lowbyte(uint16_t halfword) {
	return halfword & 0xFF;
}

_always_inline uint16_t halfword(uint8_t highbyte, uint8_t lowbyte) {
	return ((uint16_t)highbyte << 8) + lowbyte;
}

#endif // TURAG_BYTES_H
