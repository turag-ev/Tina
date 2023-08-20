/**
 *  @brief		Base class for all TURAG feldbus devices
 *  @file		device_tina.cpp
 *  @date		04.04.2013
 *  @author		Martin Oemus
 *
 */

#define TURAG_DEBUG_LOG_SOURCE "B"

#include <tina++/tina.h>

#if TURAG_USE_TURAG_FELDBUS_HOST

#include <tina++/thread.h>
#include <tina++/time.h>
#include <tina++/crc/xor.h>
#include <tina++/crc/crc.h>
#include <tina/debug/print.h>
#include <tina/math.h>
#include <cstring>

#include "device.h"


namespace TURAG {
namespace Feldbus {

namespace {

struct TURAG_PACKED DeviceInfoInternal {
    uint8_t deviceProtocolId;
    uint8_t deviceTypeId;
    union {
        struct {
            uint8_t crcType: 3;
            uint8_t deviceInfoNewVariant: 1;
            uint8_t reserved: 3;
            uint8_t packaStatisticsAvailability: 1;
        };
        uint8_t crcDataField;
    };
    union {
        struct TURAG_PACKED {
            uint16_t bufferSize;
            uint16_t reserved;
            uint8_t nameLength;
            uint8_t versioninfoLength;
        } legacy;
        struct TURAG_PACKED {
            uint16_t extendedDeviceInfoSize;
            uint32_t uuid;
        } newType;
    } variable;
    uint16_t uptimeFrequency;
};

struct TURAG_PACKED BaseRequest {
    BaseRequest() : zeroKey(0), key(0) { }

    uint8_t zeroKey;
    uint8_t key;
};

struct TURAG_PACKED PackedUint32 {
    uint32_t value;
};


}

Mutex Device::listMutex;
Device* Device::firstDevice(nullptr);


Device::Device(const char* name, unsigned address, FeldbusAbstraction& feldbus, ChecksumType type,
       unsigned int max_transmission_attempts,
       unsigned int max_transmission_errors) :
    BaseDevice(feldbus, type, max_transmission_attempts),
	dysFunctionalLog_(SystemTime::fromSec(5)),
	myNextDevice(nullptr),
	name_(name),
	maxTransmissionErrors(max_transmission_errors),
	myCurrentErrorCounter(0),
    myAddress(static_cast<uint8_t>(address)),
	hasCheckedAvailabilityYet(false)
{
	{
		Mutex::Lock lock(listMutex);

		if (firstDevice == nullptr) {
			firstDevice = this;
		} else {
			myNextDevice = firstDevice;
			firstDevice = this;
		}
	}
	if (!name_) {
		name_ = "???";
	}
}

 
bool Device::transceive(uint8_t *transmit, int transmit_length, uint8_t *receive, int receive_length, bool ignoreDysfunctional, bool transmitBroadcast)
{
	bool bailOutBecauseDysfunctional = isDysfunctional() && !ignoreDysfunctional;

	if (dysFunctionalLog_.doErrorOutput(!bailOutBecauseDysfunctional)) {
		turag_errorf("DEVICE \"%s\" DYSFUNCTIONAL. PACKAGE DROPPED (%u).", name_, dysFunctionalLog_.getErrorCount());
		dysFunctionalLog_.resetErrorCount();
	}
	if (bailOutBecauseDysfunctional) {
        return false;
	}

	// Generate target address.
	// We assume the caller wants to transmit a broadcast, if he does not supply any means to store an answer.
	// Thus we use zero rather than the device's address.
    uint8_t useAddress = !receive || !receive_length || transmitBroadcast ? TURAG_FELDBUS_BROADCAST_ADDR : myAddress;


    FeldbusAbstraction::ResultStatus status;

	// If the device is dysfunctional, we can force transmissions with
	// ignoreDysfunctional. But in this case it wouldn't make sense
	// to try more than a single transmission as the reason for a failure
	// is then most likely the device still being dysfunctional.
    if (isDysfunctional()) {
        status = BaseDevice::transceive(useAddress, transmit, transmit_length, receive, receive_length, 1);
    } else {
        status = BaseDevice::transceive(useAddress, transmit, transmit_length, receive, receive_length);
    }


    switch (status)
    {
    case FeldbusAbstraction::ResultStatus::Success:
		// if we had a successful transmission, we reset the error counter
		// but resetting the error only makes sense if we got a response from the device
		if (receive && receive_length != 0) {
			myCurrentErrorCounter = 0;

			// Because resetting the error counter essentially
			// means setting the device to be functional again
			// we also have to reset the dysFunctionalLog_ to get
			// an error message if the device fails again within
			// a short time.
			dysFunctionalLog_.resetAll();
        }
        turag_debugf("%s: rs485 transceive successful!", name_);
        return true;
    case FeldbusAbstraction::ResultStatus::TransmissionError:
        turag_warningf("%s: rs485 transceive failed: Transmission error", name_);
        myCurrentErrorCounter += 1;
        return false;
    case FeldbusAbstraction::ResultStatus::ChecksumError:
        turag_warningf("%s: rs485 transceive failed: Checksum error", name_);
        myCurrentErrorCounter += 1;
        return false;
    default:
        // C++ specifies that the range of enums (and enum classes) is the same
        // as the range of the underlying type. So without this default case
        // the compiler generates a warning.
        // Anyway, if this case occurs, something is really fucked up.
        turag_errorf("%s: rs485 transceive failed: Unknown error", name_);
        myCurrentErrorCounter += 1;
        return false;
    }
}

bool Device::sendPing(void) {
	Request<> request;
	Response<> response;

	return transceive(request, &response, true);
}

bool Device::isAvailable(bool forceUpdate) {
	if (!hasCheckedAvailabilityYet || forceUpdate) {
		while (!isDysfunctional()) {
			if (sendPing()) {
				break;
			}
		}
	}

    hasCheckedAvailabilityYet = true;
    return !isDysfunctional();
}

bool Device::getDeviceInfo(DeviceInfo* device_info) {
	if (!myDeviceInfo.isValid()) {
        Request<uint8_t> request;
        Response<DeviceInfoInternal> response;

        request.data = 0;

        if (!transceive(request, &response)) {
            return false;
        }

        if (response.data.deviceInfoNewVariant) {
            myDeviceInfo.uuid_ = response.data.variable.newType.uuid;
            myDeviceInfo.extendedDeviceInfoPacketSize_ = response.data.variable.newType.extendedDeviceInfoSize;
        } else {
            Request<BaseRequest> uuidRequest;
            uuidRequest.data.key = TURAG_FELDBUS_DEVICE_COMMAND_GET_UUID;

            Response<PackedUint32> uuidResponse;

            if (!transceive(uuidRequest, &uuidResponse)) {
                return false;
            }
            myDeviceInfo.uuid_ = uuidResponse.data.value;
            myDeviceInfo.extendedDeviceInfoPacketSize_ = 0;

            myExtendedDeviceInfo.bufferSize_ = response.data.variable.legacy.bufferSize;
            myExtendedDeviceInfo.nameLength_ = response.data.variable.legacy.nameLength;
            myExtendedDeviceInfo.versioninfoLength_ = response.data.variable.legacy.versioninfoLength;
        }

        myDeviceInfo.deviceProtocolId_ = response.data.deviceProtocolId;
		myDeviceInfo.deviceTypeId_ = response.data.deviceTypeId;
        myDeviceInfo.crcDataField_ = response.data.crcDataField;
        myDeviceInfo.uptimeFrequency_ = response.data.uptimeFrequency;

    }
    if (device_info) *device_info = myDeviceInfo;
    return true;
}

bool Device::getExtendedDeviceInfo(ExtendedDeviceInfo *extended_device_info) {
    if (!getDeviceInfo(nullptr)) {
        return false;
    }

    if (!myExtendedDeviceInfo.isValid()) {
        Request<BaseRequest> request;
        request.data.key = TURAG_FELDBUS_DEVICE_COMMAND_GET_EXTENDED_INFO;

        // because we receive a packet including address and checksum
        // we need a slightly bigger array.
        const int receive_length = myDeviceInfo.extendedDeviceInfoPacketSize() + myAddressLength + 1;
        uint8_t recvBuffer[receive_length];

        if (!transceive(reinterpret_cast<uint8_t*>(&request),
                          sizeof(request),
                          recvBuffer,
                          receive_length)) {
            return false;
        }

        myExtendedDeviceInfo.nameLength_ = recvBuffer[myAddressLength + 1];
        myExtendedDeviceInfo.versioninfoLength_ = recvBuffer[myAddressLength + 2];
        memcpy(&myExtendedDeviceInfo.bufferSize_, recvBuffer + myAddressLength + 3, 2);
    }

    if (extended_device_info) *extended_device_info = myExtendedDeviceInfo;
    return true;
}

bool Device::receiveDeviceRealName(char* out_string) {
    // It is possible that nobody called getDeviceInfo() yet,
    // so our device info would be empty. Because the device
    // info contains the length of the string we are about 
	// to receive, we make sure getDeviceInfo() gets called.
    if (!getExtendedDeviceInfo(nullptr)) {
        return false;
    }

    return receiveString(
        TURAG_FELDBUS_DEVICE_COMMAND_DEVICE_NAME,
        myExtendedDeviceInfo.nameLength(),
		out_string);
}

bool Device::receiveVersionInfo(char* out_string) {
    // It is possible that nobody called getDeviceInfo() yet,
    // so our device info would be empty. Because the device
    // info contains the length of the string we are about 
	// to receive, we make sure getDeviceInfo() gets called.
    if (!getExtendedDeviceInfo(nullptr)) {
        return false;
    }

    return receiveString(
        TURAG_FELDBUS_DEVICE_COMMAND_VERSIONINFO,
        myExtendedDeviceInfo.versionInfoLength(),
		out_string);
}

bool Device::receiveString(uint8_t command, uint8_t stringLength, char* out_string) {
    if (!out_string) {
		return false;
    }

    Request<BaseRequest> request;
    request.data.key = command;

    // because we receive a packet including address and checksum
    // we need a slightly bigger array.
    uint8_t recvBuffer[stringLength + myAddressLength + 1];

    if (!transceive(reinterpret_cast<uint8_t*>(&request),
                      sizeof(request),
                      recvBuffer,
                      stringLength + myAddressLength + 1)) {
        return false;
    }

    for (int i = 0; i < stringLength; ++i) {
        out_string[i] = recvBuffer[i + myAddressLength];
    }
    out_string[stringLength] = 0;

    return true;	
}

bool Device::receiveUptime(float* uptime) {
	if (!uptime) {
		return false;
	}
	
	if (!myDeviceInfo.isValid()) {
		if (!getDeviceInfo(nullptr)) {
			return false;
		}
	}

	if (myDeviceInfo.uptimeFrequency() == 0) {
        *uptime = NAN;
        return true;
	}
	
	uint32_t count = 0;
	
    if (!receiveErrorCount(TURAG_FELDBUS_DEVICE_COMMAND_UPTIME_COUNTER, &count)) {
		return false;
	}

	*uptime = static_cast<float>(count) / static_cast<float>(myDeviceInfo.uptimeFrequency());
    return true;
}

bool Device::receiveNumberOfAcceptedPackages(uint32_t* packageCount) {
    return receiveErrorCount(TURAG_FELDBUS_DEVICE_COMMAND_PACKAGE_COUNT_CORRECT, packageCount);
}

bool Device::receiveNumberOfOverflows(uint32_t* overflowCount) {
    return receiveErrorCount(TURAG_FELDBUS_DEVICE_COMMAND_PACKAGE_COUNT_BUFFEROVERFLOW, overflowCount);
}

bool Device::receiveNumberOfLostPackages(uint32_t* lostPackagesCount) {
    return receiveErrorCount(TURAG_FELDBUS_DEVICE_COMMAND_PACKAGE_COUNT_LOST, lostPackagesCount);
}

bool Device::receiveNumberOfChecksumErrors(uint32_t* checksumErrorCount) {
    return receiveErrorCount(TURAG_FELDBUS_DEVICE_COMMAND_PACKAGE_COUNT_CHKSUM_MISMATCH, checksumErrorCount);
}

bool Device::receiveErrorCount(uint8_t command, uint32_t* buffer) {
	if (!buffer) {
		return false;
	}
	
    Request<BaseRequest> request;
    request.data.key = command;
	
    Response<PackedUint32> response;
	
	if (!transceive(request, &response)) {
		return false;
	}
	
	*buffer = response.data.value;
	
	return true;
}

bool Device::receiveAllSlaveErrorCount(uint32_t* counts) {
	if (!counts) {
		return false;
	}
	
    Request<BaseRequest> request;
    request.data.key = TURAG_FELDBUS_DEVICE_COMMAND_PACKAGE_COUNT_ALL;
	
	struct Value {
		uint32_t packageCount;
		uint32_t overflowCount;
		uint32_t lostPackagesCount;
		uint32_t checksumErrorCount;
	} TURAG_PACKED;

	Response<Value> response;
	
	if (!transceive(request, &response)) {
		return false;
	}

	counts[0] = response.data.packageCount;
	counts[1] = response.data.overflowCount;
	counts[2] = response.data.lostPackagesCount;
	counts[3] = response.data.checksumErrorCount;
	
	return true;
}

bool Device::resetSlaveErrors(void) {
    Request<BaseRequest> request;
    request.data.key = TURAG_FELDBUS_DEVICE_COMMAND_RESET_PACKAGE_COUNT;

	Response<> response;
	
	return transceive(request, &response);
}






} // namespace Feldbus
} // namespace TURAG

#endif // TURAG_USE_TURAG_FELDBUS_HOST
