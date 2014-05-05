#define TURAG_DEBUG_LOG_SOURCE "H"

#include <ch.h>
#include <hal.h>
#include <tina/debug.h>
#include <tina/tina.h>
#include <tina/time.h>
#include <platform/chibios/backplane.h>

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


bool turag_rs485_init(uint32_t baud_rate, TuragSystemTime timeout) {
    uint8_t buf[1] = { 0 };

    chBSemInit(&_RS485_Sem, TRUE);

    serial_cfg_rs485.speed = baud_rate;
    sdStart(&RS485SD, &serial_cfg_rs485);       // CtrlM-specific
    rs485_timeout = timeout;

    // setup RTS output
    palSetPadMode(GPIOD, BPD_SC_RTS, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOD, BPD_SC_RTS);

    // flush input queue
    sdReadTimeout(&RS485SD, buf, 1, TIME_IMMEDIATE);
    sdWriteTimeout(&RS485SD, buf, 1, TIME_IMMEDIATE);

    // wait until slaves left their bootloaders
    chThdSleepMilliseconds(500);
    chBSemSignal(&_RS485_Sem);

    infof("RS485 init: %u baud, timeout %u ms -> status %d\n", (unsigned int)baud_rate, (unsigned int)timeout.value, RS485SD.state);

    if (RS485SD.state != SD_READY) {
        error("RS485 init FAILED!");
    }

    return (RS485SD.state == SD_READY);
}

bool turag_rs485_ready(void)
{
    return (RS485SD.state == SD_READY);
}

bool turag_rs485_transceive(uint8_t *input, int input_length, uint8_t *output, int output_length) {
    bool send_ok = true, recv_ok = true;

    chBSemWait(&_RS485_Sem);

    if (input && input_length > 0) {
        // activate RS485 driver for sending
        palSetPad(GPIOD, BPD_SC_RTS);
        int ok = sdWriteTimeout(&RS485SD, input, input_length, MS2ST(5));
        // TC interrupt handler sets the RTS pin after transmission is completed

        send_ok = (ok == input_length);
        debugf("turag_rs485_transceive: sending %d bytes, %d bytes written ok, driver state %d, OK: %d", input_length, ok, RS485SD.state, send_ok);
    }

    if (output && output_length > 0) {
        // read answer, timeout in systemticks!
        int ok = sdReadTimeout(&RS485SD, output, output_length, rs485_timeout.value);

        recv_ok = (ok == output_length);
        debugf("turag_rs485_transceive: receiving %d bytes, retval %d, driver state %d, OK: %d", output_length, ok, RS485SD.state, recv_ok);
    }

    chBSemSignal(&_RS485_Sem);

    return (send_ok && recv_ok);
}

void turag_rs485_buffer_clear(void) {
	uint8_t dummy[100];
	while (sdAsynchronousRead(&RS485SD, dummy, sizeof(dummy)) == sizeof(dummy));
}
