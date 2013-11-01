/**
 *  @brief		rs485 interface
 *  @file		rs485.h
 *  @date		28.10.2013
 *  @author		Martin Oemus <martin@oemus.net>
 * 
 */
#ifndef TURAG_CHIBI_RS485_H_
#define TURAG_CHIBI_RS485_H_

#include <tina/tina.h>
#include <tina/time.h>

#ifdef __cplusplus
extern "C" {
#endif


bool turag_rs485_init(uint32_t baud_rate, TuragSystemTime timeout);
bool turag_rs485_transceive(uint8_t* input, int input_length, uint8_t* output, int output_length);


#ifdef __cplusplus
}
#endif


#endif /* TURAG_CHIBI_RS485_H_ */
