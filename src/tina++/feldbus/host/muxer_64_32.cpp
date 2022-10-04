#include "muxer_64_32.h"

#include <tina/feldbus/protocol/turag_feldbus_fuer_muxer.h>


namespace TURAG {
namespace Feldbus {


struct PackedUint16 {
    uint16_t value;
} _packed;

struct PackedUint8 {
    uint8_t value;
} _packed;

struct PackedUint8Uint8 {
    uint8_t value1;
    uint8_t value2;
} _packed;

struct CmdPackedUint16 {
    uint8_t cmd;
    uint16_t value;
} _packed;

struct CmdPackedUint16Uint8Uint8 {
    uint8_t cmd;
    uint16_t value1;
    uint8_t value2;
    uint8_t value3;
} _packed;

struct CmdPackedUint8Uint8Uint8 {
    uint8_t cmd;
    uint8_t value1;
    uint8_t value2;
    uint8_t value3;
} _packed;

struct Cmd {
    uint8_t cmd;
} _packed;

struct CmdPackedUint8 {
    uint8_t cmd;
    uint8_t value;
} _packed;

struct CmdPackedUint8Uint8 {
    uint8_t cmd;
    uint8_t value1;
    uint8_t value2;
} _packed;



bool Muxer_64_32::intitialize()
{
    // retreive max cycle Length
    Request<Cmd> request;
    request.data.cmd = TURAG_FELDBUS_MUXER_MAX_CYCLE_LENGTH;

    Response<PackedUint16> response;

    bool success = transceive(request, &response);
    if (!success) return false;

    maxCycleLength = response.data.value;


    // initialize cycle length to 0
    if (!setCycleLength(0)) return false;


    // initialize trigger mode to NoTrigger
    if (!setTriggerMode(TriggerMode::NoTrigger)) return false;


    // disable output
    if (!disableOutput()) return false;

    return true;
}

bool Muxer_64_32::setCycleLength(int length)
{
    if (length < 0 || length > maxCycleLength) return false;

    Request<CmdPackedUint16> request;
    request.data.cmd = TURAG_FELDBUS_MUXER_CYCLE_LENGTH;
    request.data.value = length;

    Response<PackedUint8> response;

    bool success = transceive(request, &response);

    if (success && response.data.value == 1) {
        cycleLength = static_cast<int>(length);
        return true;
    } else {
        return false;
    }
}


bool Muxer_64_32::getCycleIndex(unsigned *index)
{
    Request<Cmd> request;
    request.data.cmd = TURAG_FELDBUS_MUXER_CYCLE_INDEX;

    Response<PackedUint16> response;

    bool success = transceive(request, &response);
    if (success) {
        *index = response.data.value;
        return true;
    } else {
        return false;
    }
}

bool Muxer_64_32::setCycleIndex(unsigned index)
{
    Request<CmdPackedUint16> request;
    request.data.cmd = TURAG_FELDBUS_MUXER_CYCLE_INDEX;
    request.data.value = static_cast<uint16_t>(index);

    Response<PackedUint8> response;

    bool success = transceive(request, &response);

    if (success && response.data.value == 1) {
        return true;
    } else {
        return false;
    }
}

bool Muxer_64_32::setTriggerMode(Muxer_64_32::TriggerMode mode)
{
    Request<CmdPackedUint8> request;
    request.data.cmd = TURAG_FELDBUS_MUXER_TRIGGER_MODE;
    request.data.value = static_cast<uint8_t>(mode);

    Response<PackedUint8> response;

    bool success = transceive(request, &response);

    if (success && response.data.value == 1) {
        triggerMode = mode;
        return true;
    } else {
        return false;
    }
}


bool Muxer_64_32::setConfig(uint16_t cycleIndex, uint8_t transmitter, uint8_t receiver)
{
    Request<CmdPackedUint16Uint8Uint8> request;
    request.data.cmd = TURAG_FELDBUS_MUXER_CONFIG;
    request.data.value1 = cycleIndex;
    request.data.value2 = transmitter;
    request.data.value3 = receiver;

    Response<PackedUint8> response;

    bool success = transceive(request, &response);

    if (success && response.data.value == 1) {
        return true;
    } else {
        return false;
    }
}

bool Muxer_64_32::clearConfig()
{
    Request<CmdPackedUint8Uint8Uint8> request;
    request.data.cmd = TURAG_FELDBUS_MUXER_CLEAR_CONFIG;
    request.data.value1 = 1;
    request.data.value2 = 1;
    request.data.value3 = 1;

    Response<> response;

    bool success = transceive(request, &response);
    return success;
}

bool Muxer_64_32::setManualOutput(uint8_t transmitter, uint8_t receiver)
{
    Request<CmdPackedUint8Uint8> request;
    request.data.cmd = TURAG_FELDBUS_MUXER_SET_MANUAL_OUTPUT;
    request.data.value1 = transmitter;
    request.data.value2 = receiver;

    Response<PackedUint8> response;

    bool success = transceive(request, &response);

    if (success && response.data.value == 1) {
        return true;
    } else {
        return false;
    }
}

bool Muxer_64_32::enableOutput()
{
    Request<Cmd> request;
    request.data.cmd = TURAG_FELDBUS_MUXER_ENABLE;

    Response<> response;

    bool success = transceive(request, &response);

    if (success) {
        outputEnabled = true;
    }
    return success;
}

bool Muxer_64_32::disableOutput()
{
    Request<Cmd> request;
    request.data.cmd = TURAG_FELDBUS_MUXER_DISABLE;

    Response<> response;

    bool success = transceive(request, &response);
    if (success) {
        outputEnabled = false;
    }
    return success;
}





} // namespace Feldbus
} // namespace TURAG
