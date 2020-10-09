/**
 *  @brief		TURAG-Feldbus für Lokalisierungssensoren protocol definitions.
 *  @file		turag_feldbus_fuer_lokalisierungssensoren.h
 *  @date		29.12.2013
 *  @author		Martin Oemus <martin@oemus.net>
 *  @ingroup	feldbus-protocol
 *
 */

#ifndef TINA_FELDBUS_PROTOCOL_TURAG_FELDBUS_FUER_LOKALISIERUNGSSENSOREN_H
#define TINA_FELDBUS_PROTOCOL_TURAG_FELDBUS_FUER_LOKALISIERUNGSSENSOREN_H


#include "turag_feldbus_bus_protokoll.h"


/**
 * @name device types
 * @{
 */
#define TURAG_FELDBUS_LOKALISIERUNGSSENSOREN_DEVICE_TYPE_COLORSENSOR			0x01
#define TURAG_FELDBUS_LOKALISIERUNGSSENSOREN_DEVICE_TYPE_ENCODER				0x02
#define TURAG_FELDBUS_LOKALISIERUNGSSENSOREN_DEVICE_TYPE_GEGI					0x03
#define TURAG_FELDBUS_LOKALISIERUNGSSENSOREN_DEVICE_TYPE_IMU_TESTBOARD			0x04
///@}


/**
 * @name color sensor
 * @{
 */
#define CMD_CLR_CLR									(5)
#define CMD_CLR_HSV									(1)
#define CMD_CLR_RGB									(2)
#define CMD_CLR_REQUEST_CONTROL_DATA_TRANSMISSION	(3)
#define CMD_CLR_TRANSMIT_CONTROL_DATA				(4)
///@}


/**
 * @name encoder
 * @{
 */
#define CMD_ENC_READ_INCREMENTS						(0)
///@}


/**
 * @name GeGi
 * @{
 */
#define CMD_GEGI_READ_DATA		(0x00)
#define CMD_GEGI_OPPO_POS		(0x01)
#define CMD_GEGI_LED			(0x02)
#define CMD_GEGI_MEASURE		(0x40)

#define CMD_GEGI_ACK			(0)		//0 for ack, other for nack

#define GEGI_BEACON_ROBO_A              (0)
#define GEGI_BEACON_ROBO_B              (1)
#define GEGI_BEACON_OPPO_A              (2)
#define GEGI_BEACON_OPPO_B              (3)

#define GEGI_FLAG_ROBO_ACTIVE_BEACON    (0)
#define GEGI_FLAG_BEACON_0              (1)
#define GEGI_FLAG_BEACON_1              (2)
#define GEGI_FLAG_BEACON_2              (3)
#define GEGI_FLAG_ROBO_PASSIVE_BEACON   (4)
#define GEGI_FLAG_OPPO_A                (5)
#define GEGI_FLAG_OPPO_B                (6)
///@}

#endif // TINA_FELDBUS_PROTOCOL_TURAG_FELDBUS_FUER_LOKALISIERUNGSSENSOREN_H
