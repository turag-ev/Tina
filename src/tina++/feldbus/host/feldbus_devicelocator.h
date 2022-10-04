#ifndef DEVICELOCATOR_H
#define DEVICELOCATOR_H


#include <tina++/tina.h>
#include <tina++/thread.h>
#include <tina++/time.h>
#include <tina++/debug/errorobserver.h>
#include <tina/feldbus/protocol/turag_feldbus_bus_protokoll.h>
#include <tina++/feldbus/host/feldbusabstraction.h>
#include "feldbus_basedevice.h"

namespace TURAG {
namespace Feldbus {


class DeviceLocator : public BaseDevice {
    DeviceLocator(const DeviceLocator&) = delete;
    DeviceLocator& operator=(const DeviceLocator&) = delete;

public:
    DeviceLocator(FeldbusAbstraction& feldbus,
           ChecksumType type = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE,
           unsigned int max_transmission_attempts = TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ATTEMPTS) :
        BaseDevice(feldbus, type, max_transmission_attempts)
    {}


    bool sendNopBroadcast(void);

    bool sendBroadcastPing(uint32_t *uuid);

    bool sendUuidPing(uint32_t uuid);

    bool receiveBusAddress(uint32_t uuid, unsigned* busAddress);

    bool setBusAddress(uint32_t uuid, unsigned busAddress);

    bool resetBusAddress(uint32_t uuid);

    bool enableBusNeighbors(void);

    bool disableBusNeighbors(void);

    bool resetAllBusAddresses(void);

    bool requestBusAssertion(int searchmaskLength, uint32_t searchAddress, bool* detectedDevice, bool onlyDevicesWithoutAddress = false);

    bool sendGotoSleepBroadcast();

private:

};



} // namespace Feldbus
} // namespace TURAG


#endif // DEVICELOCATOR_H
