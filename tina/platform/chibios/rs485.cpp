#define TURAG_DEBUG_LOG_SOURCE "H"

#include <tina++/tina.h>

#if TURAG_USE_TURAG_FELDBUS_HOST

#include <ch.h>
#include <hal.h>
#include <tina/debug.h>
#include <tina/time.h>
#include <platform/chibios/backplane.h>
#include <tina/feldbus/host/rs485.h>
#include <tina++/utils/highresdelaytimer.h>

#ifdef EUROBOT_2013
# warning "using EB13!"
# define RS485SD SD3
#elif defined(EUROBOT_2014)
# define RS485SD SD2
#else
# error "no eurobot define!"
#endif

/** USART3 --> RS485 */
static SerialConfig serial_cfg_rs485 = {
    0,
    0,
    0,
    0
};

static TuragSystemTime rs485_timeout;
static BinarySemaphore _RS485_Sem;


static TURAG::HighResDelayTimer delay;
static uint16_t bus_delay;

extern "C" bool turag_rs485_init(uint32_t baud_rate, TuragSystemTime timeout) {
    uint8_t buf[1] = { 0 };

    chBSemInit(&_RS485_Sem, TRUE);

    serial_cfg_rs485.speed = baud_rate;
    sdStart(&RS485SD, &serial_cfg_rs485);       // CtrlM-specific
    rs485_timeout = timeout;
	
    // setup RTS receive
    palSetPadMode(GPIOD, BPD_SC_RTS, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOD, BPD_SC_RTS);

    // flush transmit queue
    sdReadTimeout(&RS485SD, buf, 1, TIME_IMMEDIATE);
    sdWriteTimeout(&RS485SD, buf, 1, TIME_IMMEDIATE);

    // wait until slaves left their bootloaders
    chThdSleepMilliseconds(500);
    chBSemSignal(&_RS485_Sem);

    turag_infof("RS485 init: %u baud, timeout %u ms -> status %d\n", (unsigned int)baud_rate, (unsigned int)timeout.value, RS485SD.state);

    if (RS485SD.state != SD_READY) {
        turag_error("RS485 init FAILED!");
    }

	// calculate bus delay required for 15 frames distance of
	// TURAG feldbus [us]
	// we always round up.
	bus_delay = (15 * 1000000 - 1) / baud_rate + 1;
	delay.init();

	return (RS485SD.state == SD_READY);
}

extern "C" bool turag_rs485_ready(void) {
    return (RS485SD.state == SD_READY);
}

extern "C" bool turag_rs485_transceive(uint8_t *transmit, int* transmit_length, uint8_t *receive, int* receive_length, bool delayTransmission) {
	// prevents crashes
    while (!turag_rs485_ready()) {
        chThdSleepMilliseconds(10);
    }

    bool send_ok = true, recv_ok = true;
	uint16_t transmit_delay = 0;

	// communication on the bus should always be atomic, thus the semaphore wait.
    chBSemWait(&_RS485_Sem);
	
	// Insert bus delay, if required. The timer is started after each transmission,
	// making sure we never wait longer than necessary.
	if (delayTransmission) {
		delay.wait();
	}

	if (transmit && transmit_length && *transmit_length) {
		int transmit_length_copy = *transmit_length;
		
		// activate RS485 driver for sending
		palSetPad(GPIOD, BPD_SC_RTS);
		int ok = sdWriteTimeout(&RS485SD, transmit, transmit_length_copy, MS2ST(5));
		// TC interrupt handler sets the RTS pin after transmission is completed

		// calculate transmit time in [us]
		transmit_delay = 1000000 * 10 / serial_cfg_rs485.speed * ok;

		send_ok = (transmit_length_copy == ok);
		*transmit_length = ok;
		turag_debugf("turag_rs485_transceive: sending %d bytes, %d bytes written ok, driver state %d, OK: %d", transmit_length_copy, ok, RS485SD.state, send_ok);
    }
    
    if (!send_ok) {
		if (receive_length) {
			*receive_length = 0;
		}
		chBSemSignal(&_RS485_Sem);
		return false;
    }

	if (receive && receive_length && *receive_length) {
		int receive_length_copy = *receive_length;
		
		// read answer, timeout in systemticks!
		int ok = sdReadTimeout(&RS485SD, receive, receive_length_copy, rs485_timeout.value);

		recv_ok = (ok == receive_length_copy);
		*receive_length = ok;
		turag_debugf("turag_rs485_transceive: receiving %d bytes, retval %d, driver state %d, OK: %d", receive_length_copy, ok, RS485SD.state, recv_ok);

		// Start the delay timer after communication is finished.
		// On the next call of this function we can wait for the timeout.
		// This way we can use the required delay time for computing if there
		// is anything to be done.
		delay.startTimeout_us(bus_delay);
	} else {
		// also wait for transmission to finish if we did not receive any data
		delay.startTimeout_us(bus_delay + transmit_delay);
	}

    chBSemSignal(&_RS485_Sem);

    return (send_ok && recv_ok);
}

extern "C" void turag_rs485_buffer_clear(void) {
    while (!turag_rs485_ready()) {
        chThdSleepMilliseconds(10);
    }

    while (sdGetTimeout(&RS485SD, TIME_IMMEDIATE) != Q_TIMEOUT);
}

#endif
