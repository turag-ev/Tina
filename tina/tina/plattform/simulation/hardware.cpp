#include <plattform/hardware.h>

void Hardware_Init()                                { }
void Hardware::Init()                               { }
void Hardware::InitExtensions()                     { }
int32_t Hardware::getElectronicsVoltage()           { return 0; }
int32_t Hardware::getMechanicsVoltage()             { return 0; }
bool Hardware::getStartJumper()                     { return false; }
bool Hardware::getDistanceCherryLeft()              { return false; }
bool Hardware::getDistanceCherryRight()             { return false; }
bool Hardware::getDistanceCandleLeft()              { return false; }
bool Hardware::getDistanceCandleRight()             { return false; }
bool Hardware::getContrastCandleLeft()              { return false; }
bool Hardware::getContrastCandleRight()             { return false; }
bool Hardware::getDistanceGlassRight()              { return false; }
bool Hardware::getDistanceGlassLeft()               { return false; }
bool Hardware::enable5V()                           { return true; }
bool Hardware::disable5V()                          { return true; }
bool Hardware::is5Venabled()                        { return true; }
bool Hardware::enable7V()                           { return true; }
bool Hardware::disable7V()                          { return true; }
bool Hardware::is7Venabled()                        { return true; }
bool Hardware::enable12V()                          { return true; }
bool Hardware::disable12V()                         { return true; }
bool Hardware::is12Venabled()                       { return true; }
bool Hardware::enable24V()                          { return true; }
bool Hardware::disable24V()                         { return true; }
bool Hardware::is24Venabled()                       { return true; }
bool Hardware::setPump(uint8_t, bool)               { return true; }
bool Hardware::getPump(uint8_t)                     { return true; }
bool Hardware::isInEmergencyStop()                  { return false; }
bool Hardware::isGood24V()                          { return true; }
bool Hardware::isGood14VDrive()                     { return true; }
bool Hardware::isGood14VServo()                     { return true; }
bool Hardware::setValve(uint8_t, bool)              { return true; }
bool Hardware::getValve(uint8_t)                    { return true; }
int Hardware::getPressure(uint8_t)                  { return false; }
bool Hardware::isGood5V()                           { return true; }
bool Hardware::isGood7V()                           { return true; }
bool Hardware::isGood12V()                          { return true; }
bool Hardware::servoBoardAvailable()                { return true; }
bool Hardware::servoSetRange(const ServoIndex_t, int32_t, int32_t) { return true; }
bool Hardware::servoSetPos(const ServoIndex_t, const int32_t) { return true; }
bool Hardware::servoEnable(const ServoIndex_t)      { return true; }
bool Hardware::servoDisable(const ServoIndex_t)     { return true; }
