#ifndef MUXER_64_32_H
#define MUXER_64_32_H


#include "device.h"
#include <tina++/tina.h>
#include <tina/feldbus/protocol/turag_feldbus_fuer_muxer.h>

namespace TURAG {
namespace Feldbus {

/*
 *
 */
class Muxer_64_32: public TURAG::Feldbus::Device {
public:
    enum class TriggerMode {
        NoTrigger = TURAG_FELDBUS_MUXER_TRIGGER_MODE_NONE,
        FallingEdge = TURAG_FELDBUS_MUXER_TRIGGER_MODE_FALLING,
        RisingEdge = TURAG_FELDBUS_MUXER_TRIGGER_MODE_RISING,
        BothEdges = TURAG_FELDBUS_MUXER_TRIGGER_MODE_BOTH,
        Undefined
    };

    Muxer_64_32(const char* name, unsigned int address, FeldbusAbstraction& feldbus, ChecksumType type = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE,
            const AddressLength addressLength = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_ADDRESS_LENGTH) :
            Device(name, address, feldbus, type, addressLength), cycleLength(-1), maxCycleLength(-1), triggerMode(TriggerMode::Undefined),
            outputEnabled(false)
    {
    }

    bool intitialize();

    int getCycleLength() { return cycleLength; }
    bool setCycleLength(int length);

    int getMaxCycleLength() { return maxCycleLength; }

    bool getCycleIndex(unsigned *index);
    bool setCycleIndex(unsigned index);

    TriggerMode getTriggerMode(void) { return triggerMode; }
    bool setTriggerMode(TriggerMode mode);

    bool setConfig(uint16_t cycleIndex, uint8_t transmitter, uint8_t receiver);
    bool clearConfig();

    bool setManualOutput(uint8_t transmitter, uint8_t receiver);

    bool enableOutput();
    bool disableOutput();
    bool getOutputEnabled() { return outputEnabled; }

private:
    bool outputEnabled;
    int cycleLength;
    int maxCycleLength;
    TriggerMode triggerMode;
};

} // namespace Feldbus
} // namespace TURAG


#endif // MUXER_64_32_H
