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
    Bootloader(const char* name_, int address, ChecksumType type = TURAG_FELDBUS_DEVICE_CONFIG_STANDARD_CHECKSUM_TYPE) :
        Device(name_, address, type) {}

    bool transceiveBoot (uint8_t *transmit, int transmit_length, uint8_t *receive, int receive_length);
};

} // namespace Feldbus
} // namespace TURAG

#endif /* TURAGFELDBUSBOOTLOADER_H_ */
