#include "feldbus_binaryaddresssearcher.h"

#include <tina++/thread.h>

namespace TURAG {
namespace Feldbus {



bool BinaryAddressSearcher::TryFindNextDevice(bool* foundDevice, uint32_t* deviceAddress) {
    if (foundDevice == nullptr || deviceAddress == nullptr) {
        return false;
    }

    while (addressesToSearch.size() > 0)
    {
        BinaryAddressSearcher::SearchAddress searchAddress(addressesToSearch.front());
        addressesToSearch.pop_front();

        SystemTime timeDiff = SystemTime::now() - lastTransmissionTime;

        // Console.WriteLine("timediffSec: " + timeDiffSec);

        if (timeDiff < delayTime)
        {
            Thread_delay(delayTime - timeDiff);
        }

        lastTransmissionTime = SystemTime::now();

        bool detectedBusAssertion = false;
        bool success = deviceLocator.requestBusAssertion(
                    searchAddress.level(), searchAddress.address(), &detectedBusAssertion, onlyDevicesWithoutAddress);


        if (!success)
        {
            return false;
        }
        else
        {
            auto result = searchAddress.getNextAddresses(detectedBusAssertion);
            SearchAddress nextLevelAddresses, sameLevelAddress, detectedDevice;
            std::tie(detectedDevice, nextLevelAddresses, sameLevelAddress) = result;

            if (nextLevelAddresses.valid())
            {
                addressesToSearch.push_front(nextLevelAddresses);
            }
            if (sameLevelAddress.valid())
            {
                addressesToSearch.push_back(sameLevelAddress);
            }

            if (detectedDevice.valid())
            {
                *foundDevice = true;
                *deviceAddress = detectedDevice.address();

                // Console.WriteLine("found " + BaseDevice.FormatUuid(detectedDevice.Address));
            } else {
                *foundDevice = false;
            }

            return true;
        }
    }
}





std::tuple<BinaryAddressSearcher::SearchAddress, BinaryAddressSearcher::SearchAddress, BinaryAddressSearcher::SearchAddress>
BinaryAddressSearcher::SearchAddress::getNextAddresses(bool foundThisAddress) {
    SearchAddress nextLevelAddresses;
    SearchAddress sameLevelAddress;
    SearchAddress detectedDevice;

    if (level() == 0) {
        if (foundThisAddress) {
            nextLevelAddresses.set(address(), level() + 1);
        }
    } else {
        bool leftBranch = (address() & static_cast<uint32_t>(1 << (level() - 1))) == 0;

        if (foundThisAddress) {
            SearchAddress oneLevelDeeper(address(), level() + 1);

            if (oneLevelDeeper.found()) {
                detectedDevice = oneLevelDeeper;
            } else {
                nextLevelAddresses = oneLevelDeeper;
            }

            if (leftBranch) {
                sameLevelAddress = SearchAddress(address() | static_cast<uint32_t>(1 << (level() - 1)), level()); // sibling on same level
            }
        } else if (leftBranch) {
            SearchAddress oneLevelDeeper(address() | static_cast<uint32_t>(1 << (level() - 1)), level()); // sibling on same level

            if (oneLevelDeeper.found()) {
                detectedDevice = oneLevelDeeper;
            } else {
                nextLevelAddresses = oneLevelDeeper;
            }
        }
    }

    return std::make_tuple(detectedDevice, nextLevelAddresses, sameLevelAddress);
}


} // namespace Feldbus
} // namespace TURAG
