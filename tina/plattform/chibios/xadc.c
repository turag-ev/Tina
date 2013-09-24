#include <ch.h>
#include <hal.h>
#include "xadc.h"
#include "xserial.h"
#include "backplane2013.h"
#include <debug/cooldebug.h>
#include <stackusage.h>

/**
 * @file    xadc.c
 * @brief   ADC reader
 *
 * @{
 */

/* sample 1 channel */
#define ADC_GRP1_NUM_CHANNELS   1
/* take 8 samples per conversion series */
#define ADC_GRP1_BUF_DEPTH      8

/* adc sample storage */
static adcsample_t samples1[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];
/* adc 7V value */
static volatile uint16_t adc_mon7v = 0;

#ifndef XADC_THREAD
/*
 * ADC callback when a series of conversions is done.
 */
static void adccallback(ADCDriver *adcp, adcsample_t *buffer, size_t n) {

  (void)adcp;
  (void)n;
  adc_mon7v = (buffer[0] + buffer[1] + buffer[2] + buffer[3]) / 4;
}
#endif

//ADC callback functions, no adccallback as adc Convert wakes up the thread
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {
  (void)adcp;
  (void)err;
}

/**
 * @brief   ADC conversion group.
 * Mode:        Circular buffer, 8 samples of 1 channel.
 * Channels:    IN14.
 */
static const ADCConversionGroup adcgrpcfg1 = {
  FALSE,                     /* continuous, samples in circular buffer */
  ADC_GRP1_NUM_CHANNELS,    /* we sample one pin */
#ifdef XADC_THREAD
  NULL,
#else
  adccallback,
#endif
  adcerrorcallback,
  0,                        /* CR1 */
  ADC_CR2_SWSTART,          /* CR2 */
  ADC_SMPR1_SMP_AN14(ADC_SAMPLE_480),   /* measure for 3 cycles */
  0,                        /* SMPR2 */
  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
  0,                        /* SQR2 */
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN14)  /* measurement sequence */
};

/*
 * get value of latest measurement
 */
uint16_t adc_get_mon7v()
{
    /* measured pin voltage -> adc value:
     * 1.603V -> 1971
     * 2.507V -> 3097
     * 3.012V -> 3722
     * 3.332V -> 4092
     * => 3.332/4092*4096 = 3.33525
     */
    uint16_t val = ((uint32_t)adc_mon7v * 3335) / 4096;
    SCPLT_DEBUG("adc_get_mon7v val %d => %d mV\r\n", adc_mon7v, val);
    return val;
}

#ifdef XADC_THREAD
/**
 * @brief   ADC measurement thread
 */
static WORKING_AREA(adc_wa, 512);
static msg_t adc_thread(void *p)
{
    static int errcnt;
    msg_t res;
    (void)p;
    chRegSetThreadName("adc");

    while(1) {
        // start conversion, blocks until ADC_GRP1_BUF_DEPTH are taken
        res = adcConvert(&ADCD2, &adcgrpcfg1, samples1, ADC_GRP1_BUF_DEPTH);
        if (res == RDY_OK) {
            // average over all samples
            adc_mon7v = (samples1[0] + samples1[1] + samples1[2] + samples1[3] + \
                samples1[4] + samples1[5] + samples1[6] + samples1[7]) / 8;

            SCPLT_DEBUG("adc_thread val %d %d %d %d %d %d %d %d => %d\r\n", samples1[0], \
                samples1[1], samples1[2], samples1[3], samples1[4], samples1[5], \
                samples1[6], samples1[7], adc_mon7v);
        } else {
            if (errcnt < 10) {
                errcnt++;
                SCPLT_ERROR("adc fail #%d\r\n", errcnt);
            }
        }

        chThdSleepMilliseconds(500);
    }

    return 0;
}

size_t xadc_getStackUsage(void)
{
    return su_get_stack_usageC((const char *)&adc_wa, sizeof(adc_wa));
}

size_t xadc_getStackSize(void)
{
    return sizeof(adc_wa);
}
#endif

/**
 * @brief   ADC initialization
 */
void xadc_init()
{
    /* we measure only the voltage on BPC_MONITOR_7V_ELECTRICAL
     * which is already configured as analog input in TURAG_STM/board.h
     */

    /*
     * Activates the ADC2 driver
     */
    adcStart(&ADCD2, NULL);

#ifdef XADC_THREAD
    /** - start measurement thread */
    chThdCreateStatic(adc_wa, sizeof(adc_wa), NORMALPRIO - 3, adc_thread, NULL);
#else
    /*
     * Start a single ADC conversion.
     */
    adcStartConversion(&ADCD2, &adcgrpcfg1, samples1, ADC_GRP1_BUF_DEPTH);
#endif
}

/** @} */
