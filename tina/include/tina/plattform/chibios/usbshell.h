#ifndef USBSHELL_H_
#define USBSHELL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ch.h>
#include <hal.h>
#include <serial_usb.h>

// start usb shell non-blockingly in an own thread
//#define USB_SHELLSTARTER_THREAD

// include ChibiOS builtin shell, include CMenu otherwise
//#define USB_CHIBIOS_SHELL

void usbshell_init(void);

extern SerialUSBDriver SDU1;

#ifdef __cplusplus
}
#endif

#endif
