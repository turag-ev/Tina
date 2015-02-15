#ifndef TINA_BYTES_H
#define TINA_BYTES_H

#include <tina/tina.h>

static _always_inline uint8_t turag_highbyte(uint16_t halfword) {
	return (halfword >> 8) & 0xFF;
}

static _always_inline uint8_t turag_lowbyte(uint16_t halfword) {
	return halfword & 0xFF;
}

static _always_inline uint16_t turag_halfword(uint8_t highbyte, uint8_t lowbyte) {
	return ((uint16_t)highbyte << 8) + lowbyte;
}

#endif // TINA_BYTES_H
