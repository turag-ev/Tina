#ifndef BACKPLANE2013_H_
#define BACKPLANE2013_H_

/* import basic pin definitions for Steuerung M */
#include <steuerungm.h>

/* define macros matching the 2013 pin names on the backplane
 * syntax: BPx_name where x means GPIOx
 */

/* RS485 transceiver (USART3) */
#define BPD_SC_RX                   GPIOD_USART3RX
#define BPD_SC_TX                   GPIOD_USART3TX
#define BPD_SC_RTS                  GPIOD_USART3RTS // if you change this, change it in serial_lld.c too

/* XBEE module (USART6) */
#define BPC_XBEE_RXD                GPIOC_USART6RX
#define BPC_XBEE_TXD                GPIOC_USART6TX

/* distance sensors (digital input) */
#define BPC_DISTANCE_GLASS_LEFT     GPIOC_GPIO0_AD0
#define BPC_DISTANCE_GLASS_RIGHT    GPIOC_GPIO1_AD1
#define BPC_DISTANCE_CHERRY         GPIOC_GPIO2_AD2
#define BPC_NC_RIGHT                GPIOC_GPIO3_AD3

#define BPA_DISTANCE_CANDLE_LEFT    GPIOA_GPIO4_AD4
#define BPA_DISTANCE_CANDLE_RIGHT   GPIOA_GPIO5_AD5
#define BPA_DISTANCE_CHERRY_RIGHT   GPIOA_GPIO6_AD6
#define BPA_DISTANCE_CHERRY_LEFT    GPIOA_GPIO7_AD7

/* contrast sensors (digital input) */
#define BPB_CONTRAST_CANDLE_LEFT    GPIOB_GPIO10_AD10
#define BPB_CONTRAST_CANDLE_RIGHT   GPIOB_GPIO11_PWM0

/* battery monitor (analog input) */
#define BPC_MONITOR_7V_ELECTRICAL   GPIOC_GPIO8_AD8
/* this is ADC12_IN14 */

/* start jumper and estop (digital input) */
#define BPC_START                   GPIOC_GPIO9_AD9

/* supply enable (output) */
#define BPE_24V_ENABLE              GPIOE_GPIO12
#define BPE_5V_ENABLE               GPIOE_GPIO13
#define BPE_7V2_ENABLE              GPIOE_GPIO14
#define BPE_12V_ENABLE              GPIOE_GPIO15

/* valve enable (output) */
#define BPE_SC_VALVE8               GPIOE_GPIO16

#endif
