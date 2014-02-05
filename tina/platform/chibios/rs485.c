#define LOG_SOURCE "H"

#include <tina/debug.h>
#include <tina/tina.h>
#include <tina/time.h>
// #include <ch.h>
// #include <hal.h>
// #include <steuerungm.h>
#warning backplane2013.h should not be included here.
#include <platform/chibios/backplane2013.h>

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
    sdStart(&SD3, &serial_cfg_rs485);
    rs485_timeout = timeout;

    // setup RTS output
    palSetPadMode(GPIOD, BPD_SC_RTS, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOD, BPD_SC_RTS);

    // flush input queue
    sdReadTimeout(&SD3, buf, 1, TIME_IMMEDIATE);
    sdWriteTimeout(&SD3, buf, 1, TIME_IMMEDIATE);

    // wait until slaves left their bootloaders
    chThdSleepMilliseconds(500);
    chBSemSignal(&_RS485_Sem);

    infof("RS485 inited\n");
    return true;
}

bool turag_rs485_transceive(uint8_t *input, int input_length, uint8_t *output, int output_length) {
    bool ok = true;

    chBSemWait(&_RS485_Sem);

    if (input && input_length > 0) {
        // activate RS485 driver for sending
        palSetPad(GPIOD, BPD_SC_RTS);
        ok = sdWriteTimeout(&SD3, input, input_length, MS2ST(5));
        // TC interrupt handler sets the RTS pin after transmission is completed
    }

    if (output && output_length > 0) {
        // read answer, timeout in systemticks!
        ok = sdReadTimeout(&SD3, output, output_length, rs485_timeout.value);
    }

    chBSemSignal(&_RS485_Sem);

    return ok;
}

void turag_rs485_buffer_clear(void) {
	uint8_t dummy[100];
	while (sdAsynchronousRead(&SD3, dummy, sizeof(dummy)) == sizeof(dummy));
}