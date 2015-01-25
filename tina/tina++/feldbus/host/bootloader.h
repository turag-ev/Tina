/**
 *  @brief		TURAG feldbus bootloader base class
 *  @file		bootloader.h
 *  @date		11.11.2014
 *  @author		Florian Voelker
 *
 */


#ifndef TURAGFELDBUSBOOTLOADER_H_
#define TURAGFELDBUSBOOTLOADER_H_

#include "device.h"
#include <tina++/tina.h>
#include <tina/feldbus/protocol/turag_feldbus_fuer_bootloader.h>


namespace TURAG {
namespace Feldbus {

class Bootloader : public TURAG::Feldbus::Device {
public:
	/**
	 * \brief Konstruktor.
	 * \param[in] name_
	 * \param[in] address
	 * \param[in] type
	 * \param[in] addressLength
	 */
    Bootloader(const char* name_, unsigned address, ChecksumType type = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE,
		const AddressLength addressLength = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_ADDRESS_LENGTH) :
        Device(name_, address, type, addressLength)
    {
    }
    
    bool sendEnterBootloaderBroadcast(void);

    bool transceiveBoot (uint8_t *transmit, int transmit_length, uint8_t *receive, int receive_length);
};

} // namespace Feldbus
} // namespace TURAG

#endif /* TURAGFELDBUSBOOTLOADER_H_ */
