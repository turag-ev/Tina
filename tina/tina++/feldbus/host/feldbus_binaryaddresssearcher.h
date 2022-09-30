#ifndef BINARYADDRESSSEARCHER_H
#define BINARYADDRESSSEARCHER_H

#include <tuple>

#include <tina++/tina.h>
#include <tina++/time.h>
#include <tina++/container/container.h>
#include "feldbus_devicelocator.h"


namespace TURAG {
namespace Feldbus {


class BinaryAddressSearcher
{
private:
    class SearchAddress {
    public:
        static constexpr int MaxLevel = 32;

        static SearchAddress GetStartSearchAddress() {
            return SearchAddress(0, 0);
        }

        SearchAddress() :
            myAddress(0), myLevel(-1) {}

        SearchAddress(uint32_t address, int level) :
            myAddress(address), myLevel(level) {}

        uint32_t address() const {
            return myAddress;
        }

        int level() const {
            return myLevel;
        }

        bool found() const {
            return myLevel > MaxLevel;
        }

        bool invalid() const {
            return myLevel < 0;
        }

        bool valid() const {
            return myLevel >= 0;
        }

        void set(uint32_t address, int level) {
            myAddress = address;
            myLevel = level;
        }

        std::tuple<SearchAddress, SearchAddress, SearchAddress> getNextAddresses(bool foundThisAddress);

    private:
        uint32_t myAddress;
        int myLevel;
    };

public:
    BinaryAddressSearcher(DeviceLocator& deviceLocator_, unsigned delayTimeMs_ = 5, bool onlyDevicesWithoutAddress_ = false) :
        deviceLocator(deviceLocator_), onlyDevicesWithoutAddress(onlyDevicesWithoutAddress_), lastTransmissionTime(0)
    {
        delayTime = SystemTime::fromMsec(delayTimeMs_);
        addressesToSearch.push_back(SearchAddress::GetStartSearchAddress());
    }

    bool searchFinished() const {
        return addressesToSearch.empty();
    }

    bool TryFindNextDevice(bool* foundDevice, uint32_t* deviceAddress);

private:
    ArrayBuffer<SearchAddress, SearchAddress::MaxLevel + 1> addressesToSearch;
    SystemTime delayTime;
    DeviceLocator& deviceLocator;
    bool onlyDevicesWithoutAddress;
    SystemTime lastTransmissionTime;
};


} // namespace Feldbus
} // namespace TURAG

#endif // BINARYADDRESSSEARCHER_H
