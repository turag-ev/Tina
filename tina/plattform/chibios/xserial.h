#ifndef _XSERIAL_H
#define _XSERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ch.h>
#include <hal.h>

#include "usbshell.h"

void xserial_init(void);

/* Debug SerialDriver */
#ifdef USE_USB_CMENU
#define DEBUGSD SDU1
#else
#define DEBUGSD SD4
#endif

#ifdef __cplusplus
}
#endif

#endif
