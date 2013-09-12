#include <ch.h>
#include <hal.h>

#include <casa/casa.h>
#include <casa/private.h>

#include "xcan.h"
#include <debug/cooldebug.h>
#include <stackusage.h>

/**
 * @file    xcan.c
 * @brief   CAN configuration and RX handling
 */

/**
 * @brief   CAN configuration
 *
 * CAN Bus configuration
 * See section 27.9.2 in the STM32F4 reference manual
 *
 * This system (settings in mcuconf.h):
 *  sys clk = 128 MHz (SLOW) or 168 MHz (NORMAL)
 *  APB1 clk = sys_clk/4 = 32 MHz (SLOW) or 42 MHz (NORMAL)
 *
 * CAN bus:
 *  Baudrate = 1 Mbps
 *  SJW = 4 tq
 *
 * SLOW setting, exact timing as in the AT91s:
 *  Time Quantum length = 62.5 ns
 *  PROP_DELAY + PHASE1 = 5+4 tq
 *  PHASE2 = 6 tq
 *  SJW = 4 tq
 *  APB1 clk / BRP = 16 MHz => BRP = 2
 *
 * NORMAL setting, diverges slightly:
 *  TQ length = 71.428... ns
 *  PROP_DELAY + PHASE1 = 8 tq
 *  PHASE2 = 5 tq
 *  APB1 clk / BRP = 14 MHz => BRP = 3
 */
static const CANConfig cancfg = {
    // automatic sleep + wakeup, transmit chronologically, activate timer
    CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP | CAN_MCR_TTCM,
    // 1 mbps
#if STM_CAN_SLOW
    CAN_BTR_SJW(3) | CAN_BTR_TS2(5) | CAN_BTR_TS1(8) | CAN_BTR_BRP(1)
#else
    CAN_BTR_SJW(3) | CAN_BTR_TS2(4) | CAN_BTR_TS1(7) | CAN_BTR_BRP(2)
#endif
};

/**
 * @brief   CAN Receiver thread
 *          dispatches frames to CASA
 */
static WORKING_AREA(can_rx_wa, 6000);
static msg_t can_rx(void *p)
{
    EventListener el;
    CANRxFrame rxmsg;

    (void)p;
    chRegSetThreadName("can_rx");
    chEvtRegister(&CAND1.rxfull_event, &el, 0);
    while(!chThdShouldTerminate()) {
        if (chEvtWaitAnyTimeout(ALL_EVENTS, TIME_INFINITE) == 0)
            continue;

        while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == RDY_OK) {
#if 0
            // print packet if SCPLT debug output is active
            if (cooldebug_debug_active(CDBGSRC_SCPLATFORM)) {
                // print packet on uart
                SCPLT_DEBUG("[%6d] CANRX ", rxmsg.SYSTIME);

                if (rxmsg.IDE == CAN_IDE_EXT) {
                    SCPLT_DEBUG("EID %.8x ", rxmsg.EID);
                } else if (rxmsg.IDE == CAN_IDE_STD) {
                    SCPLT_DEBUG("SID %.3x ", rxmsg.SID);
                }

                SCPLT_DEBUG("RTR %x DLC %x ", rxmsg.RTR, rxmsg.DLC);

                if (rxmsg.DLC > 0 && rxmsg.DLC <= 8) {
                    int i;

                    SCPLT_DEBUG("DATA ");
                    for (i = 0; i < rxmsg.DLC/4; i++) {
                        SCPLT_DEBUG("%.8x ", rxmsg.data32[i]);
                    }
                }

                SCPLT_DEBUG("\r\n");
            }
#endif

            // call CASA
            if (rxmsg.IDE == CAN_IDE_EXT) {
                Casa_ProcessFrame(&rxmsg);
            }
        }
    }

    chEvtUnregister(&CAND1.rxfull_event, &el);
    return 0;
}

size_t xcan_getStackUsage(void)
{
    return su_get_stack_usageC((const char *)&can_rx_wa, sizeof(can_rx_wa));
}

size_t xcan_getStackSize(void)
{
    return sizeof(can_rx_wa);
}

/**
 * @brief   CAN initialization
 */
void xcan_init(void)
{
    /** - activate CAN driver 1 */
    canStart(&CAND1, &cancfg);

    /** - initialize CASA */
    Casa_Init();

    /** - start receiver thread */
    chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO + 20, can_rx, NULL);

    /* can pins have been configured with TURAG_STM/board.h */
    /* enable can driver => slope control tied to GND, steepest slope */
    palClearPad(GPIOE, GPIOE_CANEN);
}
