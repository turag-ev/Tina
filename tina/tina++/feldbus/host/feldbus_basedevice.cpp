#include "feldbus_basedevice.h"

#include <tina++/crc/xor.h>
#include <tina++/crc/crc.h>

namespace TURAG {
namespace Feldbus {



FeldbusAbstraction::ResultStatus BaseDevice::transceive(uint8_t address, uint8_t *transmit, int transmit_length, uint8_t *receive, int receive_length, unsigned maxAttempts)
{
    // ste address in transmit buffer
    transmit[0] = address;

    // Generate checksum for transmission.
    switch (myChecksumType) {
    case ChecksumType::xor_based:
        transmit[transmit_length - 1] = XOR::calculate(transmit, transmit_length - 1);
        break;

    case ChecksumType::crc8_icode:
        transmit[transmit_length - 1] = CRC8::calculate(transmit, transmit_length - 1);
        break;

    case ChecksumType::none:
        break;
    }


//            turag_infof("%s: transceive tx [", name);
//            for (int i = 0; i < transmit_length; ++i) {
//                turag_infof("%.2x", transmit[i]);
//            }
//            turag_info("]\n");


    FeldbusAbstraction::ResultStatus status = FeldbusAbstraction::ResultStatus::TransmissionError;
    unsigned int attempt = 0;

    if (maxAttempts == 0) {
        maxAttempts = maxTransmissionAttempts;
    }


    // we try to transmit until either
    // - the transmission succeeds and the checksum is correct or
    // - the number of transmission attempts is exceeded
    while (attempt < maxAttempts && status != FeldbusAbstraction::ResultStatus::Success) {
        int transmit_length_copy = transmit_length;
        int receive_length_copy = receive_length;

        // clear buffer from any previous failed transmissions, then send
        bus_.clearBuffer();
        status = bus_.transceive(transmit, &transmit_length_copy, receive, &receive_length_copy, address, myChecksumType);


        switch (status) {
        case FeldbusAbstraction::ResultStatus::TransmissionError:
            if (transmit_length_copy < transmit_length) {
                ++myTotalTransmitErrors;
            } else if (receive_length_copy == 0) {
                ++myTotalNoAnswerErrors;
            } else {
                ++myTotalMissingDataErrors;
            }
            break;

        case FeldbusAbstraction::ResultStatus::ChecksumError:
            ++myTotalChecksumErrors;
            break;

        case FeldbusAbstraction::ResultStatus::Success:
            break;
        }

        ++attempt;
    }
    myTotalTransmissions += attempt;


//            turag_infof("%s: transceive rx success(%x|%x) [", name, success, checksum_correct);
//            for (int j = 0; j < receive_length; ++j) {
//                turag_infof("%.2x", receive[j]);
//            }
//            turag_info("]\n");

    return status;
}

} // namespace Feldbus
} // namespace TURAG
