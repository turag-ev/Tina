#include <ch.h>
#include <hal.h>

#include <cmenu/console_menu.h>

#include "xserial.h"

// configure serial port
static SerialConfig serial_cfg_4 = {
    115200,
    0,
    USART_CR2_LINEN,
    0
};

void xserial_init(void)
{
    /*
     * Activates the Serial driver 4 = UART4 = DBG
     */
    sdStart(&SD4, &serial_cfg_4);

#ifndef USE_USB_CMENU
    CMenu_Init((SerialDriver*)&SD4);
#endif
}
