//! [Includes]
#include <avr/interrupt.h>
#include <avr/io.h>

#include <tina/feldbus/slave/feldbus.h>
//! [Includes]



//! [Definitions]
#define RTS_PIN				(2)
#define RTS_PORT			(PORTD)
#define RTS_DDR				(DDRD)

#define LED_PIN				3
#define LED_PORT			PORTD
#define LED_DDR				DDRD

#define F_CPU 				8000000
#define BAUD_RATE			500000
//! [Definitions]

//! [Helper macros]
#define RTS_ON()			{ RTS_PORT |= (1<<RTS_PIN); }
#define RTS_OFF()			{ RTS_PORT &= ~(1<<RTS_PIN); }



#define UBRR_VAL    ((F_CPU+BAUD_RATE*4)/(BAUD_RATE*8)-1)  	// clever runden
#define BAUD_REAL  	(F_CPU/(8*(UBRR_VAL+1)))     			// Reale Baudrate
#define BAUD_ERROR  ((BAUD_REAL*1000)/BAUD_RATE-1000)  		// Fehler in Promille

#if ((BAUD_ERROR > 10) || (BAUD_ERROR < -10))       		// max. +/-10 Promille Fehler
  #error "Systematischer Fehler der Baudrate grösser 1 Prozent und damit zu hoch!"
#endif
//! [Helper macros]


//! [Required hardware functions]
void turag_feldbus_hardware_init() {
	RTS_DDR |= (1<<RTS_PIN);

	UBRR0 = UBRR_VAL;
	UCSR0A = (1 << U2X0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UCSR0B |= (1 << TXEN0)|(1 << RXEN0);

	OCR2A = ((F_CPU * 15 + 8/2)/ 8 + BAUD_RATE/2) / BAUD_RATE;
	TIMSK2 = (1 << OCIE2A);
	
	LED_DDR |= (1<<LED_PIN);
	TIMSK0 |= (1<<TOIE0);
	TCCR0B |= (1<<CS02) | (1<<CS00);
}

void turag_feldbus_slave_rts_off(void) {
	RTS_OFF();
	UCSR0B |= (1 << RXEN0);
}
void turag_feldbus_slave_rts_on(void) {
	UCSR0B &= ~(1<<RXEN0);
	RTS_ON();
}
void turag_feldbus_slave_activate_dre_interrupt(void) {
	UCSR0B |= (1<<UDRIE0);
}
void turag_feldbus_slave_deactivate_dre_interrupt(void) {
	UCSR0B &= ~(1<<UDRIE0);
	UCSR0A &= ~(1<<UDRE0);
}
void turag_feldbus_slave_activate_rx_interrupt(void) {
	UCSR0B |= (1<<RXCIE0);
}
void turag_feldbus_slave_deactivate_rx_interrupt(void) {
	UCSR0B &= ~(1<<RXCIE0);
}
void turag_feldbus_slave_activate_tx_interrupt(void) {
	UCSR0B |= (1<<TXCIE0);
}
void turag_feldbus_slave_deactivate_tx_interrupt(void) {
	UCSR0B &= ~(1<<TXCIE0);
	UCSR0A &= ~(1<<TXC0);
}
void turag_feldbus_slave_start_receive_timeout (void) {
	TCNT2 = 0;
	TCCR2B = (1 << CS21);
	TIFR2 = (1 << OCF0A);
}
void turag_feldbus_slave_transmit_byte (uint8_t byte) {
	UDR0 = byte;
}
void turag_feldbus_slave_begin_interrupt_protect(void) {
	cli();
}
void turag_feldbus_slave_end_interrupt_protect(void) {
	sei();
}
void turag_feldbus_slave_toggle_led(void) {
	LED_PORT ^= (1<<LED_PIN);
}
//! [Required hardware functions]

//! [Interrupts]
ISR(USART_RX_vect) {
	turag_feldbus_slave_byte_received(UDR0);
}
ISR(USART_UDRE_vect) {
	turag_feldbus_slave_ready_to_transmit();
}
ISR(USART_TX_vect) {
	turag_feldbus_slave_transmission_complete();
}
ISR(TIMER2_COMPA_vect) {
	TCCR2B = 0;
	turag_feldbus_slave_receive_timeout_occured();
}
ISR(TIMER0_OVF_vect) {
	turag_feldbus_slave_increase_uptime_counter();
}
//! [Interrupts]
