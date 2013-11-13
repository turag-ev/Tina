/**
 *  @brief		Implements slave side TURAG feldbus support
 *  @file		feldbus.c
 *  @date		07.11.2013
 *  @author		Martin Oemus <martin@oemus.net>
 * 
 */

#include "feldbus.h"
#include <tina/crc/xor.h>
#include <tina/crc/crc8.h>

#ifndef MY_ADDR
# error MY_ADDR must be defined
#endif
#ifndef TURAG_FELDBUS_SLAVE_CONFIG_CRC_TYPE
# error TURAG_FELDBUS_SLAVE_CONFIG_CRC_TYPE must be defined
#endif
#ifndef TURAG_FELDBUS_SLAVE_CONFIG_BUFFER_SIZE
# error TURAG_FELDBUS_SLAVE_CONFIG_BUFFER_SIZE must be defined
#endif
#ifndef TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
# error TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED must be defined
#else
# if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
#  warning TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
#  if TURAG_FELDBUS_SLAVE_CONFIG_BUFFER_SIZE < 13
#   error TURAG_FELDBUS_SLAVE_CONFIG_BUFFER_SIZE < 13 and needs to be bigger.
#  endif
# endif
#endif


static void start_transmission();


static struct {
	uint8_t rxbuf[TURAG_FELDBUS_SLAVE_CONFIG_BUFFER_SIZE];
	uint8_t txbuf[TURAG_FELDBUS_SLAVE_CONFIG_BUFFER_SIZE];
	uint8_t length;
	uint8_t index;
	uint8_t overflow;
	uint8_t chksum;
#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
	volatile uint8_t transmission_active;
	uint8_t chksum_required;
#endif
} uart;



void turag_feldbus_slave_init() {
	uart.txbuf[0] = TURAG_FELDBUS_MASTER_ADDR|MY_ADDR;
	uart.index = 0;
	uart.overflow = 0;
#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
	uint8_t transmission_active = 0;
#endif

	turag_feldbus_slave_rts_off();
	turag_feldbus_slave_activate_rx_interrupt();

}


void turag_feldbus_slave_byte_received(uint8_t data) {
	uart.rxbuf[uart.index] = data;
	++uart.index;

	if (uart.index >= TURAG_FELDBUS_SLAVE_CONFIG_BUFFER_SIZE) {
		uart.index = 0;
		uart.overflow = 1;
	}

	// activate timer to recognize end of command
	turag_feldbus_slave_start_receive_timeout();
}


void turag_feldbus_slave_receive_timeout_occured() {
	if (uart.overflow) goto CLEANUP;

	// calculate checksum
#if TURAG_FELDBUS_SLAVE_CONFIG_CRC_TYPE == TURAG_FELDBUS_CHECKSUM_XOR
	if (!xor_checksum_check(uart.rxbuf, uart.index - 1, uart.rxbuf[uart.index]))
#elif TURAG_FELDBUS_SLAVE_CONFIG_CRC_TYPE == TURAG_FELDBUS_CHECKSUM_CRC8_ICODE
	if (!turag_crc8_check(uart.rxbuf, uart.index - 1, uart.rxbuf[uart.index]))
#endif
		goto CLEANUP;

	// message addressed to me?
	if (uart.rxbuf[0] == MY_ADDR) {
		if (uart.index == 2) {
			// we received a ping request -> respond with empty packet (address + checksum)
			uart.length = 1;
		} else if (uart.rxbuf[1] == 0) {
			// received a debug packet
			// TODO
		} else {
			// received some other packet --> let somebody else process it
			uart.length = turag_feldbus_slave_process_package(uart.rxbuf + 1, uart.index - 2, uart.txbuf + 1) + 1;
		}

		// calculate correct checksum and initiate transmission
#if TURAG_FELDBUS_SLAVE_CONFIG_CRC_TYPE == TURAG_FELDBUS_CHECKSUM_XOR
		uart.chksum = xor_checksum_calculate(uart.txbuf, uart.length);
#elif TURAG_FELDBUS_SLAVE_CONFIG_CRC_TYPE == TURAG_FELDBUS_CHECKSUM_CRC8_ICODE
		uart.chksum = turag_crc8_calculate(uart.txbuf, uart.length);
#endif
#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
		uart.chksum_required = 1;
#endif
		start_transmission();
	} else if (uart.rxbuf[0] == TURAG_FELDBUS_BROADCAST_ADDR) {
		turag_feldbus_slave_process_broadcast(uart.rxbuf + 1, uart.index - 2);
	}


CLEANUP:
	uart.index = 0;
	uart.overflow = 0;
}


static void start_transmission() {
#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
		uart.transmission_active = 1;
#endif
	turag_feldbus_slave_deactivate_rx_interrupt();
	turag_feldbus_slave_rts_on();
	turag_feldbus_slave_activate_dre_interrupt();
}



void turag_feldbus_slave_ready_to_transmit() {
	if (uart.index == uart.length) {
#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
		if (uart.chksum_required)
#endif
			// all data bytes are processed, now transmit checksum
			turag_feldbus_slave_transmit_byte(uart.chksum);

		turag_feldbus_slave_deactivate_dre_interrupt();
		turag_feldbus_slave_activate_tx_interrupt();
	} else {
		turag_feldbus_slave_transmit_byte(uart.txbuf[uart.index]);
		++uart.index;
	}
}

void turag_feldbus_slave_transmission_complete() {
	// turn off transmitter and release bus, turn on receiver
	turag_feldbus_slave_rts_off();
	turag_feldbus_slave_deactivate_tx_interrupt();
	turag_feldbus_slave_activate_rx_interrupt();

	uart.index = 0;
#if TURAG_FELDBUS_SLAVE_CONFIG_DEBUG_ENABLED
	uart.transmission_active = 0;
#endif
}



static uint8_t digit_to_hex(uint8_t dig) {
	dig &= 0xF;
	dig += '0';
	if (dig > '9') dig += 'A' - '0' - 10;
	return dig;
}


// public debug functions
void print_text(const char *buf) {
	while (uart.transmission_active);

	uint8_t i = 0;

	while(*buf) {
		uart.txbuf[i] = *buf;
		++i;
		++buf;

		if (i >= TURAG_FELDBUS_SLAVE_CONFIG_BUFFER_SIZE) {
			i = 1;
			break;
		}
	}
	uart.length = i;
	uart.chksum_required = 0;
	start_transmission();
}

void print_char(uint8_t x) {
	while (uart.transmission_active);

	uart.txbuf[0] = '0';
	uart.txbuf[1] = 'x';
	uart.txbuf[2] = digit_to_hex(x >> 4);
	uart.txbuf[3] = digit_to_hex(x);

	uart.length = 4;
	uart.chksum_required = 0;
	start_transmission();
}

void print_short(uint16_t x) {
	while (uart.transmission_active);

	uart.txbuf[0] = '0';
	uart.txbuf[1] = 'x';
	uart.txbuf[2] = digit_to_hex(x >> 12);
	uart.txbuf[3] = digit_to_hex(x >> 8);
	uart.txbuf[4] = digit_to_hex(x >> 4);
	uart.txbuf[5] = digit_to_hex(x);
	uart.txbuf[6] = '\n';
	uart.txbuf[7] = '\r';

	uart.length = 8;
	uart.chksum_required = 0;
	start_transmission();
}
void print_short_nn(uint16_t x) {
	while (uart.transmission_active);

	uart.txbuf[0] = '0';
	uart.txbuf[1] = 'x';
	uart.txbuf[2] = digit_to_hex(x >> 12);
	uart.txbuf[3] = digit_to_hex(x >> 8);
	uart.txbuf[4] = digit_to_hex(x >> 4);
	uart.txbuf[5] = digit_to_hex(x);

	uart.length = 6;
	uart.chksum_required = 0;
	uart.transmission_active = 1;
	start_transmission();
}

void print_sshort(int16_t x) {
	while (uart.transmission_active);

	if (x<0) {
		uart.txbuf[0] = '-';
		x=-x;
	} else uart.txbuf[0] = '+';

	uart.txbuf[1] = '0';
	uart.txbuf[2] = 'x';
	uart.txbuf[3] = digit_to_hex(x >> 12);
	uart.txbuf[4] = digit_to_hex(x >> 8);
	uart.txbuf[5] = digit_to_hex(x >> 4);
	uart.txbuf[6] = digit_to_hex(x);
	uart.txbuf[7] = '\n';
	uart.txbuf[8] = '\r';

	uart.length = 9;
	uart.chksum_required = 0;
	start_transmission();
}
void print_sshort_nn(int16_t x) {
	while (uart.transmission_active);

	if (x<0) {
		uart.txbuf[0] = '-';
		x=-x;
	} else uart.txbuf[0] = '+';

	uart.txbuf[1] = '0';
	uart.txbuf[2] = 'x';
	uart.txbuf[3] = digit_to_hex(x >> 12);
	uart.txbuf[4] = digit_to_hex(x >> 8);
	uart.txbuf[5] = digit_to_hex(x >> 4);
	uart.txbuf[6] = digit_to_hex(x);

	uart.length = 7;
	uart.chksum_required = 0;
	start_transmission();
}

void print_long(uint32_t x) {
	while (uart.transmission_active);

	uart.txbuf[0] = '0';
	uart.txbuf[1] = 'x';
	uart.txbuf[2] = digit_to_hex(x >> 28);
	uart.txbuf[3] = digit_to_hex(x >> 24);
	uart.txbuf[4] = digit_to_hex(x >> 20);
	uart.txbuf[5] = digit_to_hex(x >> 16);
	uart.txbuf[6] = digit_to_hex(x >> 12);
	uart.txbuf[7] = digit_to_hex(x >> 8);
	uart.txbuf[8] = digit_to_hex(x >> 4);
	uart.txbuf[9] = digit_to_hex(x);
	uart.txbuf[10] = '\n';
	uart.txbuf[11] = '\r';


	uart.length = 12;
	uart.chksum_required = 0;
	start_transmission();
}

void print_short_d(int16_t x) {
	while (uart.transmission_active);

	uart.txbuf[0] = ' ';
	uart.txbuf[1] = ' ';
	uart.txbuf[2] = ' ';
	uart.txbuf[3] = ' ';
	uart.txbuf[4] = ' ';
	uart.txbuf[5] = ' ';
	uart.txbuf[6] = '\n';
	uart.txbuf[7] = '\r';
	itoa(x, (char*)uart.txbuf, 10);

	uart.length = 8;
	uart.chksum_required = 0;
	start_transmission();
}

void print_slong(int32_t x) {
	while (uart.transmission_active);

	if (x<0) {
		uart.txbuf[0] = '-';
		x=-x;
	} else uart.txbuf[0] = '+';

	uart.txbuf[1] = '0';
	uart.txbuf[2] = 'x';
	uart.txbuf[3] = digit_to_hex(x >> 28);
	uart.txbuf[4] = digit_to_hex(x >> 24);
	uart.txbuf[5] = digit_to_hex(x >> 20);
	uart.txbuf[6] = digit_to_hex(x >> 16);
	uart.txbuf[7] = digit_to_hex(x >> 12);
	uart.txbuf[8] = digit_to_hex(x >> 8);
	uart.txbuf[9] = digit_to_hex(x >> 4);
	uart.txbuf[10] = digit_to_hex(x);
	uart.txbuf[11] = '\n';
	uart.txbuf[12] = '\r';

	uart.length = 13;
	uart.chksum_required = 0;
	start_transmission();
}
