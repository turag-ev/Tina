/**
 *  @brief		TURAG feldbus servo class
 *  @file		servo.h
 *  @date		22.03.2013
 *  @author		Martin Oemus
 * 
 */


#ifndef TURAGFELDBUSSERVO_H_
#define TURAGFELDBUSSERVO_H_

#include "aktor.h"

namespace TURAG {
namespace Feldbus {

/**
 * @brief Basis-Servo-Klasse.
 *
 * Diese Klasse implementiert ein Interface für den RS485-Servo.
 * Da das Interface des RS485-Servos große Überschneidungen mit dem
 * des DC-Motors hat, wird diese Schnittmenge in dieser Klasse
 * implementiert. Tatsächlich zur Benutzung vorgesehen sind Subklassen
 * dieser Klasse.
 */
class ServoBase : public TURAG::Feldbus::Aktor {
public:
	/**
	 * @brief Liest den aktuellen Winkel.
	 * @param[out] angle Puffer für den Winkel.
	 * @return True bei Erfolg, ansonsten false.
	 */
    bool getCurrentAngle(float* angle) { return getValue(RS485_STELLANTRIEBE_KEY_CURRENT_ANGLE, angle); }

	/**
	 * @brief Setzt den Sollwinkel.
	 * @param angle Sollwinkel.
	 * @return
	 */
    bool getDesiredAngle(float* angle) { return getValue(RS485_STELLANTRIEBE_KEY_DESIRED_ANGLE, angle); }
    virtual bool setDesiredAngle(float angle) { return setValue(RS485_STELLANTRIEBE_KEY_DESIRED_ANGLE, angle); }

    bool getMaxAngle(float* angle) { return getValue(RS485_STELLANTRIEBE_KEY_MAX_ANGLE, angle); }
    bool setMaxAngle(float angle) { return setValue(RS485_STELLANTRIEBE_KEY_MAX_ANGLE, angle); }

    bool getMinAngle(float* angle) { return getValue(RS485_STELLANTRIEBE_KEY_MIN_ANGLE, angle); }
    bool setMinAngle(float angle) { return setValue(RS485_STELLANTRIEBE_KEY_MIN_ANGLE, angle); }

    bool getCurrentVelocity(float* velocity) { return getValue(RS485_STELLANTRIEBE_KEY_CURRENT_VELOCITY, velocity); }

    bool getDesiredVelocity(float* velocity) { return getValue(RS485_STELLANTRIEBE_KEY_DESIRED_VELOCITY, velocity); }
    virtual bool setDesiredVelocity(float velocity) { return setValue(RS485_STELLANTRIEBE_KEY_DESIRED_VELOCITY, velocity); }

    bool getMaxVelocity(float* velocity) { return getValue(RS485_STELLANTRIEBE_KEY_MAX_VELOCITY, velocity); }
    bool setMaxVelocity(float velocity) { return setValue(RS485_STELLANTRIEBE_KEY_MAX_VELOCITY, velocity); }

    bool getCurrentCurrent(float* current) { return getValue(RS485_STELLANTRIEBE_KEY_CURRENT_CURRENT, current); }

    bool getDesiredCurrent(float* current) { return getValue(RS485_STELLANTRIEBE_KEY_DESIRED_CURRENT, current); }
    bool setDesiredCurrent(float current) { return setValue(RS485_STELLANTRIEBE_KEY_DESIRED_CURRENT, current); }

    bool getMaxCurrent(float* current) { return getValue(RS485_STELLANTRIEBE_KEY_MAX_CURRENT, current); }
    bool setMaxCurrent(float current) { return setValue(RS485_STELLANTRIEBE_KEY_MAX_CURRENT, current); }

    bool getCurrentPwm(float* pwm) { return getValue(RS485_STELLANTRIEBE_KEY_CURRENT_PWM, pwm); }

    bool getDesiredPwm(float* pwm) { return getValue(RS485_STELLANTRIEBE_KEY_DESIRED_PWM, pwm); }
    bool setDesiredPwm(float pwm) { return setValue(RS485_STELLANTRIEBE_KEY_DESIRED_PWM, pwm); }

    bool getMaxPwm(float* pwm) { return getValue(RS485_STELLANTRIEBE_KEY_MAX_PWM, pwm); }
    bool setMaxPwm(float pwm) { return setValue(RS485_STELLANTRIEBE_KEY_MAX_PWM, pwm); }

    bool getVoltage(float* voltage) { return getValue(RS485_STELLANTRIEBE_KEY_VOLTAGE, voltage); }

    virtual bool enableControl() {
        return setValue(RS485_STELLANTRIEBE_KEY_CONTROL_STATE, static_cast<int32_t>(RS485_STELLANTRIEBE_CONTROL_STATE_ENABLE));
    }
    virtual bool disableControl() {
        return setValue(RS485_STELLANTRIEBE_KEY_CONTROL_STATE, static_cast<int32_t>(RS485_STELLANTRIEBE_CONTROL_STATE_DISABLE));
    }

    virtual bool setHold(void);

    /** Status request
     * @return		True if angle reached, otherwise false.
     */
    virtual bool hasAngleReached();

    /** Status request
     * @return		True if velocity reached, otherwise false.
     */
    virtual bool hasVelocityReached();

    /** Status request
     * @return		True if current reached, otherwise false.
     */
    virtual bool hasCurrentReached();

    /** Status request
     * @return		True if error detected, otherwise false.
     */
    virtual bool hasErrorDetected();

protected:
	/**
	 * @brief Konstruktor.
	 * @param name_ Bezeichnung des Gerätes.
	 * @param address Adresse des Gerätes.
	 * @param type
	 * @param addressLength
	 */
	ServoBase(const char* name_, int address, ChecksumType type = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE,
		const AddressLength addressLength = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_ADDRESS_LENGTH) :
		Aktor(name_, address, type, addressLength) {}

};

/**
 * @brief RS485-Servo-Klasse.
 *
 * Diese Klasse bildet einen vollwertigen RS485-Servo ab.
 *
 * \see https://intern.turag.de/wiki/doku.php/id,03_elektronik;turag_feldbus;servoregler
 */
class Servo : public ServoBase {
protected:
	Aktor::Command_t command_set[24];

public:
	/**
	 * @brief Servo
	 * @param name_
	 * @param address
	 * @param type
	 * @param addressLength
	 */
    Servo(const char* name_, int address, ChecksumType type = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE,
		const AddressLength addressLength = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_ADDRESS_LENGTH) :
        ServoBase(name_, address, type, addressLength) {}

	/**
	 * @brief initialize
	 * @return
	 */
	bool initialize(void) { return populateCommandSet(command_set, 24); }

	/**
	 * @brief getCurrentFeedForward
	 * @param feedForward
	 * @return
	 */
	bool getCurrentFeedForward(float* feedForward) { return getValue(RS485_STELLANTRIEBE_KEY_CTRL_CUR_STATIC_FF, feedForward); }
	/**
	 * @brief setCurrentFeedForward
	 * @param feedForward
	 * @return
	 */
	bool setCurrentFeedForward(float feedForward) { return setValue(RS485_STELLANTRIEBE_KEY_CTRL_CUR_STATIC_FF, feedForward); }
};

} // namespace Feldbus
} // namespace TURAG

#endif /* TURAGFELDBUSSERVO_H_ */
