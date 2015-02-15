#define TURAG_DEBUG_LOG_SOURCE "H"

#include <cyg/io/io.h>
#include <cyg/hal/hal_io.h>

#include <tina/tina.h>

#if TURAG_USE_TURAG_FELDBUS_HOST

#include <tina/time.h>
#include <tina/debug/print.h>

static uint32_t rs485_baud_rate;
static int timeout_symbols;



bool turag_rs485_init(uint32_t baud_rate, TuragSystemTime timeout) {
	turag_info("initRS485\n");
	
	rs485_baud_rate = baud_rate;

	timeout_symbols = turag_ticks_to_ms(timeout) * (rs485_baud_rate / 1000);
	if (timeout_symbols > 0xffff) timeout_symbols = 0xffff;

	// usart pins are driven by peripheral
	HAL_ARM_AT91_PIO_CFG(AT91_USART_RXD1);
	HAL_ARM_AT91_PIO_CFG(AT91_USART_TXD1);
	HAL_ARM_AT91_PIO_CFG(AT91_USART_RTS1);

	int __attribute__((unused)) status_reg;

	HAL_READ_UINT32(AT91_USART1 + AT91_US_CSR, status_reg);
	HAL_WRITE_UINT32(AT91_USART1 + AT91_US_CR, AT91_US_CR_TxRESET | AT91_US_CR_RxRESET | AT91_US_CR_TxDISAB | AT91_US_CR_RxDISAB);
	HAL_WRITE_UINT32(AT91_USART1 + AT91_US_BRG, AT91_US_BAUD(rs485_baud_rate));
	HAL_WRITE_UINT32(AT91_USART1 + AT91_US_MR, 0 | AT91_US_MR_CLOCK_MCK | AT91_US_MR_LENGTH_8 | AT91_US_MR_STOP_1 | AT91_US_MR_PARITY_NONE | AT91_US_MR_MODE_NORMAL | (1<<0)); // <- RS485-Mode aktiv
	HAL_WRITE_UINT32(AT91_USART1 + AT91_US_CR, AT91_US_CR_TxENAB | AT91_US_CR_RxENAB);

	// disable interrupts
	HAL_WRITE_UINT32(AT91_USART1 + AT91_US_IDR, -1);

	return true;
}


bool turag_rs485_transceive(uint8_t* input, int* input_length, uint8_t* output, int* output_length) {
#warning THIS FUNCTION IS NOT THREADSAFE ALLTHOUGH IT IS SUPPOSED TO BE.

#error This implementation is out-dated.
	
/*	if (!input || input_length == 0) return false;

	// SENDE-TEIL
	int i = 0;
	cyg_uint32 temp = 0;

	while (i < input_length) {
		// write data bytes to transmit-register
	  	HAL_WRITE_UINT32(AT91_USART1 + AT91_US_THR, (cyg_uint8)input[i]);

		// wait for raising TXRDY-flag -> character transmitted
		do {
			HAL_READ_UINT32(AT91_USART1 + AT91_US_CSR,temp);
			temp &= AT91_US_CSR_TxRDY;
		} while (temp == 0);
		
		i++;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////
	// EMPFANGS-TEIL

	if (!output || output_length == 0) return true;

	for (i = 0; i < output_length; ++i) output[i] = 0;

	i = 0;
	bool leave_loop = false;

    // disable DMA (driver doesn't work otherwise)
    HAL_WRITE_UINT32(AT91_USART1 + AT91_US_CR, AT91_US_CR_RxRESET);
    HAL_WRITE_UINT32(AT91_USART1 + AT91_US_PTCR, AT91_US_PTCR_RXTDIS);
    HAL_WRITE_UINT32(AT91_USART1 + AT91_US_CR, AT91_US_CR_RSTATUS);
    HAL_WRITE_UINT32(AT91_USART1 + AT91_US_CR, AT91_US_CR_RxENAB);

	// load initial timeout into TO-Register
	HAL_WRITE_UINT16(AT91_USART1 + AT91_US_RTO, timeout_symbols);
	HAL_WRITE_UINT32(AT91_USART1 + AT91_US_CR, AT91_US_CR_RETTO);

	while (!leave_loop) {
		// wait for raising RXRDY-flag or timeout
		do {
			HAL_READ_UINT32(AT91_USART1 + AT91_US_CSR, temp);
		} while ( ! (temp  & (AT91_US_CSR_RxRDY | AT91_US_CSR_TIMEOUT | AT91_US_CSR_RXBRK | AT91_US_CSR_OVRE | AT91_US_CSR_OVRE)));

		// only leave loop on timeout or error
		if (temp & (AT91_US_CSR_TIMEOUT | AT91_US_CSR_RXBRK | AT91_US_CSR_OVRE | AT91_US_CSR_OVRE)) {
			leave_loop = true;
		}
		if (temp & AT91_US_CSR_RxRDY) {
			HAL_READ_UINT32(AT91_USART1 + AT91_US_RHR, temp);

			// fill buffer up to the top
			if (i < output_length) {
				output[i] = temp;
			}

			++i;

			// reduce timeout after first received byte
			if (i==1) HAL_WRITE_UINT16(AT91_USART1 + AT91_US_RTO, 15);
		}
	}

	if (i == output_length) {
		return true;
	} else {
		return false;
	}*/
}

#endif

