#ifndef TINA_ECOS_RS485_H
#define TINA_ECOS_RS485_H

#include <tina/tina.h>
#include <tina/time.h>

#ifdef __cplusplus
extern "C" {
#endif

bool turag_rs485_init(uint32_t baud_rate, TuragSystemTime timeout);

// This function is required to be THREADSAFE !!!
bool turag_rs485_transceive(uint8_t* input, int input_length, uint8_t* output, int output_length);

void turag_rs485_buffer_clear(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_ECOS_RS485_H
