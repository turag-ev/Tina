/**
 *  @brief		Base class for all TURAG feldbus devices
 *  @file		device.h
 *  @date		22.03.2013
 *  @author		Martin Oemus
 *
 */


#ifndef TURAGFELDBUSDEVICE_H_
#define TURAGFELDBUSDEVICE_H_

#include <memory>

#include <tina++/tina.h>
#include <tina++/thread.h>
#include <tina/feldbus/protocol/turag_feldbus_bus_protokoll.h>


// -------------------------------------------------------------
// - config
// -------------------------------------------------------------
// these values could be relocated to an external header to make
// the whole thing configurable.

// number of trials before we let the transmission fail
#define TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ATTEMPTS		5

// specifies how many unsuccessful transmission attempts in a row are accepted
// before defining the device as dysfunctional
#define TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ERRORS			35

#define TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE			TURAG::Feldbus::Device::ChecksumType::xor_based



namespace TURAG {
namespace Feldbus {

template<typename T = void>
struct Broadcast {
	uint8_t address; // must be 0
	uint8_t id;
	T data;
	uint8_t checksum;
} _packed;

template<typename T = void>
struct Request {
	uint8_t address;
	T data;
	uint8_t checksum;
} _packed;

template<typename T = void>
struct Response {
	uint8_t address;
	T data;
	uint8_t checksum;
} _packed;

/// for zero size requests/responces

template<>
struct Broadcast<void> {
	uint8_t address; // must be 0
	uint8_t id;
	uint8_t checksum;
} _packed;

template<>
struct Request<void> {
	uint8_t address;
	uint8_t checksum;
} _packed;

template<>
struct Response<void> {
	uint8_t address;
	uint8_t checksum;
} _packed;

/*
 *
 */
class Device {
public:
	enum class ChecksumType {
		xor_based = TURAG_FELDBUS_CHECKSUM_XOR,
		crc8_icode = TURAG_FELDBUS_CHECKSUM_CRC8_ICODE,
	};

private:
	const unsigned int maxTransmissionAttempts;
	const unsigned int maxTransmissionErrors;

	ChecksumType myChecksumType;
	unsigned int myTransmissionErrorCounter;

	static unsigned int globalTransmissionErrorCounter;
	static Mutex rs485_mutex;
	static SystemTime lastTransmission;

protected:
	const unsigned int myAddress;
	bool hasCheckedAvailabilityYet;

	Device(const char* name_, unsigned int address, ChecksumType type = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE) :
		maxTransmissionAttempts(TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ATTEMPTS),
		maxTransmissionErrors(TURAG_FELDBUS_DEVICE_CONFIG_MAX_TRANSMISSION_ERRORS),
		myChecksumType(type),
		myTransmissionErrorCounter(0),
		myAddress(address),
		hasCheckedAvailabilityYet(false),
		name(name_) { }

	virtual ~Device() {}

	template<typename T, typename U> _always_inline
	bool transceive(Request<T>& transmit, Response<U>* receive) {
	transmit.address = myAddress;
	return transceive(static_cast<uint8_t*>(static_cast<void*>(std::addressof(transmit))),
										sizeof(Request<T>),
										static_cast<uint8_t*>(static_cast<void*>(receive)),
										sizeof(Response<U>));
	}

	template<typename T, typename U> _always_inline
	bool transceive(Broadcast<T>& transmit) {
		transmit.address = TURAG_FELDBUS_BROADCAST_ADDR;
		return transceive(static_cast<uint8_t*>(static_cast<void*>(std::addressof(transmit))),
											sizeof(Broadcast<T>), nullptr, 0);
	}

	// this funtion will always overwrite the last byte with the checksum of the preceeding ones
	// so take care of the right buffer size!
	bool transceive(uint8_t *transmit, int transmit_length, uint8_t *receive, int receive_length);

	bool hasReachedTransmissionErrorLimit(void) const { return myTransmissionErrorCounter > maxTransmissionErrors; }
	void clearTransmissionErrors(void) { myTransmissionErrorCounter = 0; }

public:
	const char* name;

	unsigned int getAddress(void) const { return myAddress; }
	virtual bool isAvailable(void);
};

} // namespace Feldbus
} // namespace TURAG

#endif /* TURAGFELDBUSDEVICE_H_ */
