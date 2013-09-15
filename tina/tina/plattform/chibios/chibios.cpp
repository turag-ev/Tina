#include <ch.h>
#include <hal.h>

#include <steuerungm.h>
#include <casa/casa.h>
#include <robotio/robotio.h>
#include <debug/cooldebug.h>
#include "backplane2013.h"

extern "C" {
#include "usbshell.h"
#include "xserial.h"
#include "xcan.h"
#include "xadc.h"
#include "pwmled.h"
}

#ifdef USE_CRASHRECOVERY
#include "crashrecovery.h"
#endif

#include "plattform/hardware.h"
#include "systemcontrol.h"

extern void user_code_start();

extern "C"
int main() {
    /* initialize system */
    halInit();
    chSysInit();

    ErrorLed_Set(false);
    Led_SetColor(COLOR_OFF);

    /* init serial ports and start shell */
    usbshell_init();
    xserial_init();

    Led_SetColor(COLOR_GREEN);
    chThdSleepMilliseconds(50);

    /* init CAN/CASA */
    xcan_init();
    chThdSleepMilliseconds(50);

    /* init ADC supply monitor */
    xadc_init();
    chThdSleepMilliseconds(50);

    /* shiny leds */
    pwmled_init();

#ifdef USE_CRASHRECOVERY
    /* crash recovery */
    crashrecovery_init();

    if (crashrecovery_get_crashed()) {
        // we had a crash
        SCPLT_ERROR("CRASH DETECTED!\r\n");
        SCPLT_ERROR("Recovery data:\r\n");
        crashrecovery_dump();

        // ... do some crash recovery stuff

        // clear crash data after crash recovery
        crashrecovery_clear();
    }
#endif // USE_CRASHRECOVERY

    /* start user threads */
    user_code_start();

    /* sleep */
    while (1) {
        chThdSleepMilliseconds(500);

#ifdef USE_CRASHRECOVERY
        // write recovery data. better do this in systemcontrol thread
        crashrecovery_write();
#endif // USE_CRASHRECOVERY

        chThdSleepMilliseconds(500);
    }

    return 0;
}
