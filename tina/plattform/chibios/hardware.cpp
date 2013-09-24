#define LOG_SOURCE "H"
#include <tina/debug.h>

#include "plattform/hardware.h"
#include "backplane2013.h"
#include "xadc.h"

#include <steuerungm.h>
#include <robotio/board.h>
#include <robotio/robotio.h>
#include <debug/cooldebug.h>
#include <comm/rs485bus-SystemControl.h>

namespace TURAG
{

extern "C"
void Hardware_Init()
{
    // extension board communication
    // this function may take a while if boards are not present or responsive
    RobotIO_Init();

    // local pin configuration (overrides robot-specific pin defines of TURAG_STM/board.h)
    Hardware::Init();

    // extension board pin config
    Hardware::InitExtensions();

    // RS485 transceiver init
    Hardware::initRS485();

    // bluetooth module
    Hardware::initUART();
}

void Hardware::Init()
{
    /** Backplane specific pin settings.
     * Override some pin modes defined in TURAG_STM/board.h.
     * By default all pins are set to their primary function as described by
     * their signal name, e.g. GPIOB_I2CBP_SCL is an open-drain pin
     * with AF id 4 (I2C, see datasheet DM00037051.pdf, p. 59).
     **/

    /* Backplane RS485 transceiver on USART3 uses only RX/TX/RTS, but not CTS/CK */
    palSetPadMode(GPIOD, GPIOD_USART3CK, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOD, GPIOD_USART3CTS, PAL_MODE_INPUT_PULLUP);

    /* onboard RS485 transceiver unused, deactivate */
    palSetPadMode(GPIOD, GPIOD_USART2RTS, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOD, GPIOD_USART2TX, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOD, GPIOD_USART2RX, PAL_MODE_INPUT_PULLUP);

    /* radio module on USART6 doesn't use CK */
    palSetPadMode(GPIOC, GPIOC_USART6CK_PWM3, PAL_MODE_INPUT_PULLUP);

    /* Backplane 2013 specific pin settings */
    palSetPadMode(GPIOC, BPC_DISTANCE_CHERRY, PAL_MODE_INPUT);
    palSetPadMode(GPIOC, BPC_NC_RIGHT, PAL_MODE_INPUT_PULLUP);/* unconnected, add pull-up */
    palSetPadMode(GPIOC, BPC_DISTANCE_GLASS_LEFT, PAL_MODE_INPUT);
    palSetPadMode(GPIOC, BPC_DISTANCE_GLASS_RIGHT, PAL_MODE_INPUT);

    palSetPadMode(GPIOA, BPA_DISTANCE_CHERRY_LEFT, PAL_MODE_INPUT);
    palSetPadMode(GPIOA, BPA_DISTANCE_CHERRY_RIGHT, PAL_MODE_INPUT);
    palSetPadMode(GPIOA, BPA_DISTANCE_CANDLE_LEFT, PAL_MODE_INPUT);
    palSetPadMode(GPIOA, BPA_DISTANCE_CANDLE_RIGHT, PAL_MODE_INPUT);

    palSetPadMode(GPIOB, BPB_CONTRAST_CANDLE_LEFT, PAL_MODE_INPUT);
    palSetPadMode(GPIOB, BPB_CONTRAST_CANDLE_RIGHT, PAL_MODE_INPUT);

    palSetPadMode(GPIOC, BPC_START, PAL_MODE_INPUT);

    palSetPadMode(GPIOE, BPE_24V_ENABLE, PAL_MODE_OUTPUT_OPENDRAIN);
    palSetPadMode(GPIOE, BPE_5V_ENABLE, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOE, BPE_7V2_ENABLE, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOE, BPE_12V_ENABLE, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOE, BPE_SC_VALVE8, PAL_MODE_OUTPUT_OPENDRAIN);

    /* deactivate all supplies */
    disable24V();
    disable12V();
    disable7V();
    enable5V();

    Hardware::setValve(8, false);
}

void Hardware::InitExtensions()
{
    int i, fails = 0;

    /// *** initialize IO Board pins
    infof("Configuring IO Board pins\n");

    if (RobotIO::IOBoards[0].isAvailable()) {
        infof("Init IO BOARD 0\n");

        // setup pumps, deactivate all
        for (i = 0; i < 8; i++)
            fails += !RobotIO::IOBoards[0].setupDigitalOutput(i, false);

        if (fails)
            errorf("IOBoard 0 fail: setupDigitalOutput!\n");

        // setup analog inputs, ioboard 0
        fails = 0;
        fails += !RobotIO::IOBoards[0].setupAnalogInput(IOBOARD0_AIN_EMERGENCY_STOP, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[0].setupAnalogInput(IOBOARD0_AIN_14V_DRIVE, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[0].setupAnalogInput(IOBOARD0_AIN_14V_SERVO, RobotIO::AREF_5V);

        /* btw, you can specify the measurement alignment, but ALIGN_RIGHT is the default value.
         * example: RobotIO::IOBoards[0].setupAnalogInput(IOBOARD0_AIN_24V_LASER, RobotIO::AREF_5V, RobotIO::ALIGN_RIGHT);
         *
         * btw2, the IO boards only support AREF_5V and AREF_2V5!
         */

        if (fails)
            errorf("IOBoard 0 fail: setupAnalogInput!\n");
    } else {
        errorf("IOBoard 0 not available!\n");
    }

    if (RobotIO::IOBoards[1].isAvailable()) {
        // setup valves, all shut to absorb glasses
        fails = 0;
        for (i = 0; i < 8; i++)
            fails += !RobotIO::IOBoards[1].setupDigitalOutput(i, false);

        if (fails)
            errorf("IOBoard 1 fail: setupDigitalOutput!\n");

        // setup analog inputs, ioboard 1
        fails = 0;
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_PRESSURE0, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_PRESSURE1, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_PRESSURE2, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_PRESSURE3, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_PRESSURE4, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_PRESSURE5, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_PRESSURE6, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_PRESSURE7, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_24V_LASER, RobotIO::AREF_5V);
		fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_5V_GM, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_7V_GM, RobotIO::AREF_5V);
        fails += !RobotIO::IOBoards[1].setupAnalogInput(IOBOARD1_AIN_12V_GM, RobotIO::AREF_5V);

        if (fails)
            errorf("IOBoard 1 fail: setupAnalogInput!\n");
    } else {
        errorf("IOBoard 1 not available!\n");
    }
}

/** distance sensors */
bool Hardware::getDistanceCherryLeft() {
    return !palReadPad(GPIOA, BPA_DISTANCE_CHERRY_LEFT);
}

bool Hardware::getDistanceCherryRight() {
    return !palReadPad(GPIOA, BPA_DISTANCE_CHERRY_RIGHT);
}

bool Hardware::getDistanceCandleLeft() {
    return palReadPad(GPIOA, BPA_DISTANCE_CANDLE_LEFT);
}

bool Hardware::getDistanceCandleRight() {
    return palReadPad(GPIOA, BPA_DISTANCE_CANDLE_RIGHT);
}

bool Hardware::getDistanceGlassLeft() {
    return palReadPad(GPIOC, BPC_DISTANCE_GLASS_LEFT);
}

bool Hardware::getDistanceGlassRight() {
    return palReadPad(GPIOC, BPC_DISTANCE_GLASS_RIGHT);
}

bool Hardware::getDistanceCherry() {
    return !palReadPad(GPIOC, BPC_DISTANCE_CHERRY);
}

/** contrast sensors */
bool Hardware::getContrastCandleLeft() {
    return palReadPad(GPIOB, BPB_CONTRAST_CANDLE_LEFT);
}

bool Hardware::getContrastCandleRight() {
    return palReadPad(GPIOB, BPB_CONTRAST_CANDLE_RIGHT);
}

/** start jumper */
bool Hardware::getStartJumper() {
    return !palReadPad(GPIOC, BPC_START);
}

/** supply voltage */
int32_t Hardware::getElectronicsVoltage() {
    return (int)(((uint32_t)adc_get_mon7v() * 42)/15);
}

/** stub: this pin has been disconnected */
int32_t Hardware::getMechanicsVoltage() {
    return 0;
}

/** enable/disable supply */
// we can't return isGood in the enable/disable functions
// because these functions are called in Init() when the analog pins are not yet init'ed

// 5V
bool Hardware::enable5V() {
    palSetPad(GPIOE, BPE_5V_ENABLE);
    return true;
}

bool Hardware::disable5V() {
    palClearPad(GPIOE, BPE_5V_ENABLE);
    return true;
}

bool Hardware::is5Venabled() {
    return palReadPad(GPIOE, BPE_5V_ENABLE);
}

// 7V
bool Hardware::enable7V() {
    palSetPad(GPIOE, BPE_7V2_ENABLE);
    return true;
}

bool Hardware::disable7V() {
    palClearPad(GPIOE, BPE_7V2_ENABLE);
    return true;
}

bool Hardware::is7Venabled() {
    return palReadPad(GPIOE, BPE_7V2_ENABLE);
}

// 12V
bool Hardware::enable12V() {
    palSetPad(GPIOE, BPE_12V_ENABLE);
    return true;
}

bool Hardware::disable12V() {
    palClearPad(GPIOE, BPE_12V_ENABLE);
    return true;
}

bool Hardware::is12Venabled() {
    return palReadPad(GPIOE, BPE_12V_ENABLE);
}

// 24V
bool Hardware::enable24V() {
    /* 24V enable is low active */
    palClearPad(GPIOE, BPE_24V_ENABLE);
    return true;
}

bool Hardware::disable24V() {
    palSetPad(GPIOE, BPE_24V_ENABLE);
    return true;
}

bool Hardware::is24Venabled() {
    return !palReadPad(GPIOE, BPE_24V_ENABLE);
}

/** USART3 --> RS485 */
static SerialConfig serial_cfg_rs485 = {
    RS485_BAUD_RATE,
    0,
    0,
    0
};

static BinarySemaphore _RS485_Sem;

bool Hardware::initRS485() {
    uint8_t buf[1] = { 0 };

    chBSemInit(&_RS485_Sem, TRUE);

    sdStart(&SD3, &serial_cfg_rs485);

    // setup RTS output
    palSetPadMode(GPIOD, BPD_SC_RTS, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOD, BPD_SC_RTS);

    // flush input queue
    sdReadTimeout(&SD3, buf, 1, TIME_IMMEDIATE);
    sdWriteTimeout(&SD3, buf, 1, TIME_IMMEDIATE);

    // wait until slaves left their bootloaders
    chThdSleepMilliseconds(500);
    chBSemSignal(&_RS485_Sem);

    infof("RS485 inited\n");
    return true;
}

/**
 * this functions wants a timeout in system ticks!
 */
bool Hardware::transceiveRS485(uint8_t *input, int input_length, uint8_t *output, int output_length, SystemTime timeout) {
    bool ok = true;

    chBSemWait(&_RS485_Sem);

    if (input && input_length > 0) {
        // activate RS485 driver for sending
        palSetPad(GPIOD, BPD_SC_RTS);
        ok = sdWriteTimeout(&SD3, input, input_length, MS2ST(5));
        // TC interrupt handler sets the RTS pin after transmission is completed
    }

    if (output && output_length > 0) {
        // read answer
        ok = sdReadTimeout(&SD3, output, output_length, timeout.value);
    }

    chBSemSignal(&_RS485_Sem);

    return ok;
}

/** USART6 --> Bluetooth module */
static SerialConfig serial_cfg_radio = {
    115200,
    0,
    USART_CR2_LINEN,
    0
};

bool Hardware::initUART() {
    sdStart(&SD6, &serial_cfg_radio);
    return true;
}

/**
 * @brief   read from Bluetooth UART
 * @param   max_input_length    maximum number of bytes to read
 * @param   timeout     in microseconds, 0 for none, -1000 for infinity
 * @param   bytes_read  pointer to an integer with number of read bytes
 * @returns false       if 0 bytes were read and timeout happened.
 *          true        if any bytes were read.
 */
bool Hardware::readUART(uint8_t *input, int max_input_length, SystemTime timeout, int *bytes_read) {
    int bread = 0;

    // invalid params?
    if (!input || max_input_length <= 0)
        return false;

    // read
    bread = chnReadTimeout(&SD6, input, max_input_length, timeout.value);


    /*for(int i = 0; i < bread; i++)
    	infof("reading:  %02i:%i:%c",i,input[i],input[i]);*/

    // read count wanted?
    if (bytes_read != nullptr)
        *bytes_read = bread;

    // return true if any number of bytes was read
    return (bool)bread;
}

/**
 * @brief   write to Bluetooth UART
 * @param   output_length   maximum number of bytes to write
 * @param   timeout     in microseconds, 0 for none, -1000 for infinity
 * @param   bytes_written   pointer to an integer with number of written bytes
 * @returns false       if timeout happened.
 *          true        if all bytes were written.
 */
bool Hardware::writeUART(uint8_t *output, int output_length, SystemTime timeout, int *bytes_written) {
    int bwritten = 0;

    // invalid params?
    if (!output || output_length <= 0)
        return false;

    // write
    bwritten = chnWriteTimeout(&SD6, output, output_length, timeout.value);

    /*for(int i = 0; i < bwritten; i++)
    	infof("writing: %02i:%i:%c",i,output[i],output[i]);*/

    // check for bytes written count
    if (bytes_written != nullptr)
        *bytes_written = bwritten;

    // return true if all bytes were written
    return (bwritten == output_length);
}

/** Extension Board - Analog Inputs (IOBoard 0) */

#include <tina/time.h>
static int buffered_value = 0;
static SystemTime last_read;

bool Hardware::isInEmergencyStop() {
  // buffer value to avoid i2c flooding
  if (ticks_to_ms(get_current_tick() - last_read) > 25) {
    last_read = get_current_tick();
    buffered_value = RobotIO::IOBoards[0].getAnalogPin(IOBOARD0_AIN_EMERGENCY_STOP);
  }

  if (buffered_value < 0) {// -1 if not inited, -2 on transmit error
    infof("isInEmergencyStop: ad %d\n", buffered_value);
    return true;
  }

  return buffered_value < threshold_low;
}

bool Hardware::isGood24V() {
    int val = RobotIO::IOBoards[1].getAnalogPin(IOBOARD1_AIN_24V_LASER);
//    infof("isGood24V: ad %d\n", val);
    return val > threshold_high;
}

bool Hardware::isGood14VDrive() {
    int val = RobotIO::IOBoards[0].getAnalogPin(IOBOARD0_AIN_14V_DRIVE);
//    infof("isGood14VDrive: ad %d\n", val);
    return val > threshold_high;
}

bool Hardware::isGood14VServo() {
    int val = RobotIO::IOBoards[0].getAnalogPin(IOBOARD0_AIN_14V_SERVO);
//    infof("isGood14VServo: ad %d\n", val);
    return val > threshold_high;
}

/** Extension Board - Analog Inputs (IOBoard 1) */

bool Hardware::isGood5V() {
    int val = RobotIO::IOBoards[1].getAnalogPin(IOBOARD1_AIN_5V_GM);
//    infof("isGood5V: ad %d\n", val);
    return val > threshold_high;
}

bool Hardware::isGood7V() {
    int val = RobotIO::IOBoards[1].getAnalogPin(IOBOARD1_AIN_7V_GM);
//    infof("isGood7V: ad %d\n", val);
    return val > threshold_high;
}

bool Hardware::isGood12V() {
    int val = RobotIO::IOBoards[1].getAnalogPin(IOBOARD1_AIN_12V_GM);
//    infof("isGood12V: ad %d\n", val);
    return val > threshold_high;
}

// pressure inputs are below in the pump section

/** Extension Board 1: 8 Valves
 * valves are connected to second io board (idx=1)
 */

bool Hardware::setValve(uint8_t num, bool state) {
    if (num == 8) {
        // valve 8 is directly connected to us
        palWritePad(GPIOE, BPE_SC_VALVE8, !state);

        return true; // this cannot fail
    } else if (num > 7) {
        return false;
    }

    // setting the pin can always fail if the i2c bus is congested
    // also setDigitalPin only sends i2c packets if the pin state in changed
    return RobotIO::IOBoards[1].setDigitalPin(num, state);
}

bool Hardware::getValve(uint8_t num) {
    if (num == 8) {
        // valve 8 is directly connected to us
        return !palReadPad(GPIOE, BPE_SC_VALVE8);
    } else if (num > 7) {
        return false;
    }

    // getDigitalPin returns our own cached state of the pin, so doing this is fast
    return RobotIO::IOBoards[1].getDigitalPin(num);
}

/** Extension Board 0: 8 Pumps
 * pumps are connected to first io board (idx=0)
 * pumps are low-active, so we invert all states
 */

bool Hardware::setPump(uint8_t num, bool state) {
    if (num > 7)
        return false;

    return RobotIO::IOBoards[0].setDigitalPin(num, state);
}

bool Hardware::getPump(uint8_t num) {
    if (num > 7)
        return false;

    return RobotIO::IOBoards[0].getDigitalPin(num);
}

int Hardware::getPressure(uint8_t num) {
    if (num > 7)
        return -2;

    // returns -1 on fail
    int val = RobotIO::IOBoards[1].getAnalogPin(IOBOARD1_AIN_PRESSURE0 + num);
    return val; // TODO: possibly some scaling from voltage to pressure
}

/** ServoBoard functions, wrappers around RobotIO */
bool Hardware::servoBoardAvailable(void) {
    return RobotIO::ServoBoards[0].isAvailable();
}

bool Hardware::servoSetRange(const ServoIndex_t idx, int32_t min, int32_t max) {
    return RobotIO::ServoBoards[0].setRange(idx, min, max);
}

bool Hardware::servoSetPos(const ServoIndex_t idx, const int32_t pos) {
    return RobotIO::ServoBoards[0].setPos(idx, pos);
}

bool Hardware::servoEnable(const ServoIndex_t idx) {
    return RobotIO::ServoBoards[0].enable(idx);
}

bool Hardware::servoDisable(const ServoIndex_t idx) {
    return RobotIO::ServoBoards[0].disable(idx);
}

bool Hardware::servoSetRamp(const ServoIndex_t idx, uint32_t delta_pos, uint32_t delta_t) {
	return RobotIO::ServoBoards[0].setRamp(idx, delta_pos, delta_t);
}

/** IOBoard functions */
bool Hardware::ioBoard0Available(void) {
    return RobotIO::IOBoards[0].isAvailable();
}

bool Hardware::ioBoard1Available(void) {
    return RobotIO::IOBoards[1].isAvailable();
}

} // namespace TURAG

