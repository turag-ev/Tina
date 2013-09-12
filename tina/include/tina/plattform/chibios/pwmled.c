#include <ch.h>
#include <hal.h>

#include "backplane2013.h"
#include "pwmled.h"

static PWMConfig pwmcfg = {
    10000,  /* 100Khz PWM clock frequency */
    200,    /* period */
    NULL,   /* No callback */
    /* Only channel 4 enabled */
    {
        {PWM_OUTPUT_DISABLED, NULL},
#if PWM_LED_REVERSE_POLARITY
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
#else
        {PWM_OUTPUT_ACTIVE_LOW, NULL},
        {PWM_OUTPUT_ACTIVE_LOW, NULL},
        {PWM_OUTPUT_ACTIVE_LOW, NULL},
#endif
    },
    0
};

/* this threads blinks the led to show we're not crashed */
static WORKING_AREA(blinker_wa, 256);
static msg_t blinker(void *p)
{
    (void)p;
    int i, j;
    chRegSetThreadName("ledfader");

    while (1) {
        // i = led, j = brightness
        for (i = 1; i <= 3; i++) {
            for (j = 0; j <= 50; j++) {
                pwmEnableChannel(&PWMD4, i, j*2);
                chThdSleepMilliseconds(30);
            }
            for (j = 0; j <= 50; j++) {
                pwmEnableChannel(&PWMD4, (i+1)%3+1, j*2);
                chThdSleepMilliseconds(30);
            }
            for (j = 49; j >= 0; j--) {
                pwmEnableChannel(&PWMD4, i, j*2);
                chThdSleepMilliseconds(30);
            }
            for (j = 49; j >= 0; j--) {
                pwmEnableChannel(&PWMD4, (i+1)%3+1, j*2);
                chThdSleepMilliseconds(30);
            }
        }
    }

    return 0;
}

void pwmled_init()
{
    palSetPadMode(GPIOD, GPIOD_LED1R, PAL_MODE_ALTERNATE(2));
    palSetPadMode(GPIOD, GPIOD_LED1G, PAL_MODE_ALTERNATE(2));
    palSetPadMode(GPIOD, GPIOD_LED1B, PAL_MODE_ALTERNATE(2));

    pwmStart(&PWMD4, &pwmcfg);
    pwmEnableChannel(&PWMD4, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 0));
    pwmEnableChannel(&PWMD4, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 0));
    pwmEnableChannel(&PWMD4, 3, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 0));

    /* led fader */
    chThdCreateStatic(blinker_wa, sizeof(blinker_wa), NORMALPRIO-5, blinker, NULL);
}
