#define LOG_SOURCE "H"
#include "utils/debug.h"
#include <util/ecoshelpers.h>

#include "plattform/hardware.h"
#include "im/internal_mechanics.h"

#define TO_BOOT 500


void Hardware_Init(void)
{
	cyg_thread_delay(MS_TO_TICK(TO_BOOT));
	info("Starting Hardware init");
	Hardware::Init();
	info("basics initialized");
	Hardware::InitExtensions();
	info("Extensionboards initialized");
}

void Hardware::Init()
{
	//configure 24V enable pin and disable 24V
	Pin_Setup(PIN_24V_ENABLE, Pin_Mode_DOut, PIN_DOUTFLAG_OPENDRAIN, true);
	//configure 12V enable pin and disable 12V
	Pin_Setup(PIN_12V_ENABLE, Pin_Mode_DOut, 0, false);
	//configure 5V enable pin and disable 5V
	Pin_Setup(PIN_5V_ENABLE, Pin_Mode_DOut, 0, false);
	//configure 7V enable pin and disable 7V
	Pin_Setup(PIN_7V_ENABLE, Pin_Mode_DOut, 0, false);
	//configure ADC for electronics voltage measurement
	Pin_Setup(VOLTAGE_ELECTRONICS_SUPPLY, Pin_Mode_AIn, 0, 3300);
	//Configure digital input pins for leuze distance sensors
	Pin_Setup(PIN_DISTANCE_CANDLE_LEFT, Pin_Mode_DIn, 0, 0);
	Pin_Setup(PIN_DISTANCE_CANDLE_RIGHT, Pin_Mode_DIn, 0, 0);
	Pin_Setup(PIN_DISTANCE_CHERRY_LEFT, Pin_Mode_DIn, 0, 0);
	Pin_Setup(PIN_DISTANCE_CHERRY_RIGHT, Pin_Mode_DIn, 0, 0);
	Pin_Setup(PIN_CONTRAST_CANDLE_LEFT, Pin_Mode_DIn, 0, 0);
	Pin_Setup(PIN_CONTRAST_CANDLE_RIGHT, Pin_Mode_DIn, 0, 0);
	Pin_Setup(PIN_DISTANCE_GLASS_LEFT, Pin_Mode_DIn, 0, 0);
	Pin_Setup(PIN_DISTANCE_GLASS_RIGHT, Pin_Mode_DIn, 0, 0);

	Pin_Setup(PIN_VALVE8, Pin_Mode_DOut, PIN_DOUTFLAG_OPENDRAIN, true);

	initRS485();
}

void Hardware::InitExtensions()
{
	//IO Board 1
	Pin_Setup(PIN_PUMP0, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_PUMP1, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_PUMP2, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_PUMP3, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_PUMP4, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_PUMP5, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_PUMP6, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_PUMP7, Pin_Mode_DOut, 0, false);

	Pin_Setup(PIN_MONITOR_EMERGENCY_STOP, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_MONITOR_14V_DRIVE, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_MONITOR_14V_SERVO, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_MONITOR_24V_LASER, Pin_Mode_AIn, 0, 5000);

	//IO Board 2
	Pin_Setup(PIN_VALVE0, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_VALVE1, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_VALVE2, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_VALVE3, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_VALVE4, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_VALVE5, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_VALVE6, Pin_Mode_DOut, 0, false);
	Pin_Setup(PIN_VALVE7, Pin_Mode_DOut, 0, false);

	Pin_Setup(PIN_PRESSURE0, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_PRESSURE1, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_PRESSURE2, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_PRESSURE3, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_PRESSURE4, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_PRESSURE5, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_PRESSURE6, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_PRESSURE7, Pin_Mode_AIn, 0, 5000);

	Pin_Setup(PIN_5V_GOOD_MONITOR, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_7V_GOOD_MONITOR, Pin_Mode_AIn, 0, 5000);
	Pin_Setup(PIN_12V_GOOD_MONITOR, Pin_Mode_AIn, 0, 5000);

	Pin_Setup(PIN_START_JUMPER, Pin_Mode_DIn, 0, 0);
}

int32_t Hardware::getElectronicsVoltage()
{
	int32_t value = 0;
	Pin_AInGetVoltage(VOLTAGE_ELECTRONICS_SUPPLY, (cyg_int32*) &value);
	return (value * 42)/15 + 300;
}

int32_t Hardware::getMechanicsVoltage()
{
	int32_t value = 0;
	Pin_AInGetVoltage(PIN_MONITOR_EMERGENCY_STOP, (cyg_int32*) &value);
	value = (value * 49)/12;
	return value;
}

bool Hardware::getDistanceCherryLeft()
{
	int level;
	Pin_DInGetLevel(PIN_DISTANCE_CHERRY_LEFT, &level);
	return level == 0;
}
bool Hardware::getDistanceCherryRight()
{
	int level;
	Pin_DInGetLevel(PIN_DISTANCE_CHERRY_RIGHT, &level);
	return level == 0;
}
bool Hardware::getDistanceCandleLeft()
{
	int level;
	Pin_DInGetLevel(PIN_DISTANCE_CANDLE_LEFT, &level);
	return !level == 0;
}
bool Hardware::getDistanceCandleRight()
{
	int level;
	Pin_DInGetLevel(PIN_DISTANCE_CANDLE_RIGHT, &level);
	return !level == 0;
}
bool Hardware::getContrastCandleLeft()
{
	int level;
	Pin_DInGetLevel(PIN_CONTRAST_CANDLE_LEFT, &level);
	return !level == 0;
}
bool Hardware::getContrastCandleRight()
{
	int level;
	Pin_DInGetLevel(PIN_CONTRAST_CANDLE_RIGHT, &level);
	return !level == 0;
}
bool Hardware::getDistanceGlassRight()
{
	int level;
	Pin_DInGetLevel(PIN_DISTANCE_GLASS_RIGHT, &level);
	return !level == 0;
}
bool Hardware::getDistanceGlassLeft()
{
	int level;
	Pin_DInGetLevel(PIN_DISTANCE_GLASS_LEFT, &level);
	return !level == 0;
}
bool Hardware::enable5V()
{
	Pin_DOutSetLevel(PIN_5V_ENABLE, true);
	return true;
}
bool Hardware::disable5V()
{
	Pin_DOutSetLevel(PIN_5V_ENABLE, false);
	return true;
}
bool Hardware::is5Venabled()
{
	return Pin_DOutGetLevel(PIN_5V_ENABLE);
}
bool Hardware::enable7V()
{
	Pin_DOutSetLevel(PIN_7V_ENABLE, true);
	return true;
}
bool Hardware::disable7V()
{
	Pin_DOutSetLevel(PIN_7V_ENABLE, false);
	return true;
}
bool Hardware::is7Venabled()
{
	return Pin_DOutGetLevel(PIN_7V_ENABLE);
}
bool Hardware::enable12V()
{
	Pin_DOutSetLevel(PIN_12V_ENABLE, true);
	return true;
}
bool Hardware::disable12V()
{
	Pin_DOutSetLevel(PIN_12V_ENABLE, false);
	return true;
}
bool Hardware::is12Venabled()
{
	return Pin_DOutGetLevel(PIN_12V_ENABLE);
}
bool Hardware::enable24V()
{
	Pin_DOutSetLevel(PIN_24V_ENABLE, false);
	return true;
}
bool Hardware::disable24V()
{
	Pin_DOutSetLevel(PIN_24V_ENABLE, true);
	return true;
}
bool Hardware::is24Venabled()
{
	return !Pin_DOutGetLevel(PIN_24V_ENABLE);
}
bool Hardware::setPump(uint8_t num, bool state)
{
	if(num > 7) return false;
    Pin_DOutSetLevel(PIN_PUMP0 + num, state);
	return true;
}
bool Hardware::getPump(uint8_t num)
{
	if(num > 7) return false;
	return Pin_DOutGetLevel(PIN_PUMP0 + num);
}
bool Hardware::isInEmergencyStop() {
	if (cyg_current_time() < S_TO_TICK(3)) return false;

	int value;
	Pin_AInGetVoltage(PIN_MONITOR_EMERGENCY_STOP, &value);
	return value < threshold_low;
}

bool Hardware::isGood24V()
{
	int value;
	Pin_AInGetVoltage(PIN_MONITOR_24V_LASER, &value);
	if(value > threshold_high)	return true;
	return false;
}
bool Hardware::isGood14VDrive()
{
	int value;
	Pin_AInGetVoltage(PIN_MONITOR_14V_DRIVE, &value);
	if(value > threshold_high)	return true;
	return false;
}
bool Hardware::isGood14VServo()
{
	int value;
	Pin_AInGetVoltage(PIN_MONITOR_14V_SERVO, &value);
	if(value > threshold_high)	return true;
	return false;
}
bool Hardware::setValve(uint8_t num, bool state)
{
	if(num > 8) return false;
	if(num == 8) Pin_DOutSetLevel(PIN_VALVE8, !state);
	else		Pin_DOutSetLevel(PIN_VALVE0 + num, state);
	return true;
}
bool Hardware::getValve(uint8_t num)
{
	if(num > 8) return false;
	if(num == 8) return !Pin_DOutGetLevel(PIN_VALVE8);
	return Pin_DOutGetLevel(PIN_VALVE0 + num);
}
int Hardware::getPressure(uint8_t num)
{
	if(num > 7)	return false;
	int value;
	Pin_AInGetVoltage(PIN_PRESSURE0 + num, &value);
	return value;
}
bool Hardware::isGood5V()
{
	int value;
	Pin_AInGetVoltage(PIN_5V_GOOD_MONITOR, &value);
	if(value > threshold_high)	return true;
	return false;
}
bool Hardware::isGood7V()
{
	int value;
	Pin_AInGetVoltage(PIN_7V_GOOD_MONITOR, &value);
	if(value > threshold_high)	return true;
	return false;
}
bool Hardware::isGood12V()
{
	int value;
	Pin_AInGetVoltage(PIN_12V_GOOD_MONITOR, &value);
	if(value > threshold_high)	return true;
	return false;
}

/** ServoBoard functions */
bool Hardware::servoBoardAvailable(void) {
    return (Ext_GetState(2) == Ext_State_Ready);
}

bool Hardware::servoSetRange(const ServoIndex_t idx, int32_t min, int32_t max) {
    Servo_SetRange(idx, min, max);
    return true;
}

bool Hardware::servoSetPos(const ServoIndex_t idx, const int32_t pos) {
    Servo_SetPos(idx, pos);
    return true;
}

bool Hardware::servoEnable(const ServoIndex_t idx) {
    Servo_SetEnabled(idx, true); // returns void
    return true;
}

bool Hardware::servoDisable(const ServoIndex_t idx) {
	printf("disable\n");
    Servo_SetEnabled(idx, false);
    return true;
}

bool Hardware::getStartJumper() {
	int level;
	Pin_DInGetLevel(PIN_START_JUMPER, &level);
	return level == 0;

}

/** IoBoard functions */
bool Hardware::ioBoard0Available(void) {
	return (Ext_GetState(0) == Ext_State_Ready);
}

bool Hardware::ioBoard1Available(void) {
	return (Ext_GetState(1) == Ext_State_Ready);
}

/** stub functions for compatibility */

bool Hardware::initUART() {
    return false;
}

bool Hardware::readUART(uint8_t *input, int input_length, int timeout, int *bytes_read) {
    return false;
}

bool Hardware::writeUART(uint8_t *output, int output_length, int timeout, int *bytes_written) {
    return false;
}

bool Hardware::getDistanceCherry() {
    return false;
}
