#ifndef TINA_FELDBUS_PROTOCOL_BLDC_PROTOCOL_H
#define TINA_FELDBUS_PROTOCOL_BLDC_PROTOCOL_H

#define TURAG_FELDBUS_BLDC_START    0x1
#define TURAG_FELDBUS_BLDC_STOP     0x2
#define TURAG_FELDBUS_BLDC_VELOCITY 0x3
#define TURAG_FELDBUS_BLDC_STATUS   0x4
#define TURAG_FELDBUS_BLDC_UPDATE   0x5

#define TURAG_FELDBUS_BLDC_FLAG_STOPPED 0x0
#define TURAG_FELDBUS_BLDC_FLAG_RUNNING 0x1

#endif // TINA_FELDBUS_PROTOCOL_BLDC_PROTOCOL_H
