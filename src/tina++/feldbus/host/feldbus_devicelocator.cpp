#include "feldbus_devicelocator.h"

namespace TURAG {
namespace Feldbus {



bool DeviceLocator::sendNopBroadcast()
{
    Broadcast<> request;
    request.id = 0x00;

    return transceive(0, request) == FeldbusAbstraction::ResultStatus::Success;
}

bool DeviceLocator::sendBroadcastPing(uint32_t* uuid) {
    Broadcast<uint8_t> request;
    request.id = 0x00;
    request.data = 0x00;

    // this seems to be required for proper alignment
    struct Value {
        uint32_t uuid;
    } TURAG_PACKED;

    Response<Value> response;

    if (transceive(0, request, &response) != FeldbusAbstraction::ResultStatus::Success) {
        return false;
    }

    *uuid = response.data.uuid;
    return true;
}

bool DeviceLocator::sendUuidPing(uint32_t uuid) {
    struct Value {
        uint8_t key;
        uint32_t uuid;
    } TURAG_PACKED;

    Broadcast<Value> request;

    request.id = 0x00;
    request.data.key = 0x00;
    request.data.uuid = uuid;

    Response<> response;

    return transceive(0, request, &response) == FeldbusAbstraction::ResultStatus::Success;
}

bool DeviceLocator::receiveBusAddress(uint32_t uuid, unsigned* busAddress) {
    struct Value {
        uint8_t key;
        uint32_t uuid;
        uint8_t key2;
    } TURAG_PACKED;

    Broadcast<Value> request;

    request.id = 0x00;
    request.data.key = 0x00;
    request.data.uuid = uuid;
    request.data.key2 = 0x00;

    Response<uint8_t> response;

    if (transceive(0, request, &response) != FeldbusAbstraction::ResultStatus::Success) {
        return false;
    }

    *busAddress = response.data;
    return true;
}

bool DeviceLocator::setBusAddress(uint32_t uuid, unsigned busAddress) {
    struct Value {
        uint8_t key;
        uint32_t uuid;
        uint8_t key2;
        uint8_t busAddress;
    } TURAG_PACKED;

    struct Value2 {
        uint8_t key;
        uint32_t uuid;
        uint8_t key2;
        uint16_t busAddress;
    } TURAG_PACKED;

    Broadcast<Value> request;

    request.id = 0x00;
    request.data.key = 0x00;
    request.data.uuid = uuid;
    request.data.key2 = 0x00;
    request.data.busAddress = busAddress & 0xFF;

    Response<uint8_t> response;

    if (transceive(0, request, &response) != FeldbusAbstraction::ResultStatus::Success) {
        return false;
    }

    return response.data == 1;
}

bool DeviceLocator::resetBusAddress(uint32_t uuid) {
    struct Value {
        uint8_t key;
        uint32_t uuid;
        uint8_t key2;
    } TURAG_PACKED;

    Broadcast<Value> request;

    request.id = 0x00;
    request.data.key = 0x00;
    request.data.uuid = uuid;
    request.data.key2 = 0x01;

    Response<> response;

    return transceive(0, request, &response) == FeldbusAbstraction::ResultStatus::Success;
}

bool DeviceLocator::enableBusNeighbors(void) {
    Broadcast<uint8_t> request;
    request.id = 0x00;
    request.data = 0x01;

    return transceive(0, request) == FeldbusAbstraction::ResultStatus::Success;
}
bool DeviceLocator::disableBusNeighbors(void) {
    Broadcast<uint8_t> request;
    request.id = 0x00;
    request.data = 0x02;

    return transceive(0, request) == FeldbusAbstraction::ResultStatus::Success;
}

bool DeviceLocator::resetAllBusAddresses(void) {
    Broadcast<uint8_t> request;
    request.id = 0x00;
    request.data = 0x03;

    return transceive(0, request) == FeldbusAbstraction::ResultStatus::Success;
}

bool DeviceLocator::sendGotoSleepBroadcast()
{
    Broadcast<uint8_t> request;
    request.id = 0x00;
    request.data = 0x06;

    return transceive(0, request) == FeldbusAbstraction::ResultStatus::Success;
}

bool DeviceLocator::requestBusAssertion(int searchmaskLength, uint32_t searchAddress, bool *detectedDevice, bool onlyDevicesWithoutAddress)
{
    struct Value {
        uint8_t key;
        uint8_t searchMaskLength;
        uint32_t searchAddress;
    } TURAG_PACKED;


    if (searchmaskLength < 0 || searchmaskLength > 32)
    {
        return false;
    }

    Broadcast<Value> request;
    request.id = 0x00;
    request.data.key = onlyDevicesWithoutAddress ? 0x05 : 0x04;
    request.data.searchMaskLength = static_cast<uint8_t>(searchmaskLength);
    request.data.searchAddress = searchAddress;

    // Console.WriteLine("search mask length " + searchmaskLength + " search address " + BaseDevice.FormatUuid(searchAddress));

    uint8_t receiveBuffer;

    FeldbusAbstraction::ResultStatus result = transceive(
                0,
                reinterpret_cast<uint8_t*>(&(request)), sizeof(request),
                &receiveBuffer, 1,
                1);

    // above we told the transport that we want to receive 1 byte, although we don't care what exactly.
    // Obviously the crc check will fail, which is ok. The only real failure mode here is when sending fails,
    // but the current transport implementation doesn't give us this isolated info. So currently we can never
    // fail, while a crc error or success indicate a detected bus assertion and the transport error
    // indicates a not-detected assertion.
    *detectedDevice = result != FeldbusAbstraction::ResultStatus::TransmissionError;
    return true;
}


} // namespace Feldbus
} // namespace TURAG
