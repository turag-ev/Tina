/**
 *  @brief		TURAG feldbus bootloader base class
 *  @file		bootloader_tina.cpp
 *  @date		11.11.2014
 *  @author		Florian Voelker
 *
 */

#define TURAG_DEBUG_LOG_SOURCE "B"

#include <tina++/tina.h>
#if TURAG_USE_TURAG_FELDBUS_HOST

#include "bootloader.h"

#include <tina/debug.h>

#include <algorithm>
#include <cstring>
#include <vector>



namespace TURAG {
namespace Feldbus {

	
bool Bootloader::sendEnterBootloaderBroadcast(void) {
    Device::Broadcast<uint8_t> broadcast;
    broadcast.id = TURAG_FELDBUS_DEVICE_PROTOCOL_BOOTLOADER;
    broadcast.data = TURAG_FELDBUS_BOOTLOADER_COMMAND_ENTER_BOOTLOADER;

    return transceive(broadcast);
}

bool Bootloader::sendStartProgramBroadcast(void) {
    Device::Broadcast<uint8_t> broadcast;
    broadcast.id = TURAG_FELDBUS_DEVICE_PROTOCOL_BOOTLOADER;
    broadcast.data = TURAG_FELDBUS_BOOTLOADER_COMMAND_START_PROGRAMM;

    return transceive(broadcast);
}

uint16_t Bootloader::getMcuId(void) {
	if (myMcuId == TURAG_FELDBUS_BOOTLOADER_MCU_ID_INVALID) {
		struct McuId {
			uint16_t id;
		} TURAG_PACKED;

		Device::Request<uint8_t> request;
		request.data = TURAG_FELDBUS_BOOTLOADER_COMMAND_GET_MCUID;
		
		Device::Response<McuId> response;
		
		if (transceive(request, &response)) {
			myMcuId = response.data.id;
		}
	}
	return myMcuId;
}


bool Bootloader::unlockBootloader(void) {
	getMcuId();
	
	if (myMcuId == TURAG_FELDBUS_BOOTLOADER_MCU_ID_INVALID) {
		turag_errorf("%s: tried to call Bootloader::unlockBootloader, but couldn't read MCU-ID", name());
		return false;
	}
	
	struct UnlockBootloader {
		uint8_t key;
		uint16_t mcuId;
		uint16_t unlockCode;
	} TURAG_PACKED;

	Device::Request<UnlockBootloader> request;
	request.data.key = TURAG_FELDBUS_BOOTLOADER_COMMAND_UNLOCK_BOOTLOADER;
	request.data.mcuId = myMcuId;
	request.data.unlockCode = TURAG_FELDBUS_BOOTLOADER_UNLOCK_CODE;
	
	Device::Response<uint8_t> response;
	
	if (!transceive(request, &response)) {
		return false;
	} else {
		if (response.data == TURAG_FELDBUS_BOOTLOADER_RESPONSE_UNLOCKED) {
			unlocked = true;
			return true;
		} else {
			turag_infof("%s: bootloader unlock rejected", name());
			return false;
		}
	}
}

bool Bootloader::receiveString(uint8_t command, uint8_t stringLength, char* out_string) {
    if (!out_string) {
        return false;
    }

    // because we receive a packet including address and checksum
    // we need a slightly bigger array.
    char recvBuffer[stringLength + myAddressLength + 1];


    uint8_t request[myAddressLength + 1 + 1];
    request[myAddressLength] = command;

    if (!transceive(request,
                      sizeof(request),
                      reinterpret_cast<uint8_t*>(recvBuffer),
                      stringLength + myAddressLength + 1)) {
        return false;
    }

    for (int i = 0; i < stringLength; ++i) {
        out_string[i] = recvBuffer[i + myAddressLength];
    }
    out_string[stringLength] = 0;

    return true;
}

bool Bootloader::receiveMcuIdString(char* out_string) {
    Device::Request<uint8_t> request;
    request.data = TURAG_FELDBUS_BOOTLOADER_COMMAND_RECEIVE_MCU_STRING_LENGTH;

    Device::Response<uint8_t> response;

    if (transceive(request, &response)) {
        return receiveString(
            TURAG_FELDBUS_BOOTLOADER_COMMAND_RECEIVE_MCU_STRING,
            response.data,
            out_string);
    } else {
        return false;
    }
}



uint16_t BootloaderAvrBase::getPageSize(void) {
	if (myPageSize == 0) {
		struct PageSize {
			uint16_t size;
		} TURAG_PACKED;

		Device::Request<uint8_t> request;
		request.data = TURAG_FELDBUS_BOOTLOADER_AVR_GET_PAGE_SIZE;
		
		Device::Response<PageSize> response;
		
		if (transceive(request, &response)) {
			myPageSize = response.data.size;
		}
	}
	return myPageSize;
}

uint32_t BootloaderAvrBase::getFlashSize(bool writable) {
	struct GetFlashSize {
		uint8_t key;
		uint8_t writable;
	};
	
	if (writable) {
		if (myWritableFlashSize == 0) {
			struct FlashSize {
				uint32_t size;
			} TURAG_PACKED;

			Device::Request<GetFlashSize> request;
			request.data.key = TURAG_FELDBUS_BOOTLOADER_AVR_GET_FLASH_SIZE;
			request.data.writable = 1;
			
			Device::Response<FlashSize> response;
			
			if (transceive(request, &response)) {
				myWritableFlashSize = response.data.size;
			}
		}
		return myWritableFlashSize;
	} else {
		if (myFlashSize == 0) {
			struct FlashSize {
				uint32_t size;
			} TURAG_PACKED;

			Device::Request<GetFlashSize> request;
			request.data.key = TURAG_FELDBUS_BOOTLOADER_AVR_GET_FLASH_SIZE;
			request.data.writable = 0;
			
			Device::Response<FlashSize> response;
			
			if (transceive(request, &response)) {
				myFlashSize = response.data.size;
			}
		}
		return myFlashSize;
	}
}

BootloaderAvrBase::ErrorCode BootloaderAvrBase::writeFlash(uint32_t byteAddress, uint32_t length, uint8_t* data) {
	if (!data) {
		return ErrorCode::invalid_args;
	}
	getPageSize();
	if (myPageSize == 0) {
		turag_errorf("%s: tried to call BootloaderAtmega::writePage, but couldn't read page size", name());
		return ErrorCode::preconditions_not_met;
	}
	getFlashSize(true);
	if (myWritableFlashSize == 0) {
		turag_errorf("%s: tried to call BootloaderAtmega::writePage, but couldn't read writable flash size", name());
		return ErrorCode::preconditions_not_met;
	}
	if (myWritableFlashSize - byteAddress < length) {
		turag_errorf("%s: in call to BootloaderAtmega::writePage: write would exceed writable flash boundary", name());
		return ErrorCode::invalid_args;
	}
	if (byteAddress % myPageSize != 0) {
		turag_errorf("%s: in call to BootloaderAtmega::writePage: address must be page-aligned", name());
		return ErrorCode::invalid_args;
	}
	
	unsigned pages = length / myPageSize;
	if (length % myPageSize) {
		++pages;
	}
	
    uint8_t request[myAddressLength + 1 + 4 + myPageSize + 1];
    request[myAddressLength] = TURAG_FELDBUS_BOOTLOADER_AVR_PAGE_WRITE;
	
    uint8_t response[myAddressLength + 1 + 1];
	
    byteAddress |= getFlashBaseAddress();
	uint32_t targetAddress = byteAddress;
	
	struct uint32_t_packed {
		uint32_t value;
	} TURAG_PACKED;
	
	for (unsigned i = 0; i < pages; ++i) {
        reinterpret_cast<uint32_t_packed*>(request + myAddressLength + 1)->value = targetAddress;

		uint16_t currentPageSize = std::min(myPageSize, static_cast<uint16_t>(byteAddress + length - targetAddress));
        memcpy(request + myAddressLength + 5, data, currentPageSize);
		
		unsigned k = 0;
		for (k = 0; k < maxTriesForWriting; ++k) {
            if (!transceive(request, sizeof(request), response, sizeof(response))) {
				return ErrorCode::transceive_error;
			}
            if (response[myAddressLength] == TURAG_FELDBUS_BOOTLOADER_AVR_RESPONSE_SUCCESS) {
				break;
            } else if (response[myAddressLength] == TURAG_FELDBUS_BOOTLOADER_AVR_RESPONSE_FAIL_CONTENT) {
				continue;
			} else {
                return static_cast<ErrorCode>(response[myAddressLength]);
			}
		}
		// we give up after a few tries.
		if (k == maxTriesForWriting) {
			return ErrorCode::content_mismatch;
		}
		
		targetAddress += myPageSize;
		data += myPageSize;
	}
	
	return ErrorCode::success;
}

BootloaderAvrBase::ErrorCode BootloaderAvrBase::readFlash(uint32_t byteAddress, uint32_t length, uint8_t* buffer) {
	if (!buffer) {
		return ErrorCode::invalid_args;
	}
	getFlashSize(false);
	if (myFlashSize == 0) {
		turag_errorf("%s: tried to call BootloaderAtmega::readData, but couldn't read flash size", name());
		return ErrorCode::preconditions_not_met;
	}
    if (!getExtendedDeviceInfo(nullptr)) {
		turag_errorf("%s: tried to call BootloaderAtmega::readData, but couldn't read device info", name());
		return ErrorCode::preconditions_not_met;
	}
	if (myFlashSize - byteAddress < length) {
		turag_errorf("%s: in call to BootloaderAtmega::readData: read would exceed flash size", name());
		return ErrorCode::invalid_args;
	}
	
    uint32_t packetSize = myExtendedDeviceInfo.bufferSize() - myAddressLength - 1 - 1;
	
	unsigned packets = length / packetSize;
	if (length % packetSize) {
		++packets;
	}

    uint8_t request[myAddressLength + 1 + 4 + 2 + 1];
    request[myAddressLength] = TURAG_FELDBUS_BOOTLOADER_AVR_DATA_READ;
	
    byteAddress |= getFlashBaseAddress();
    uint32_t targetAddress = byteAddress;
	
	struct header_packed {
		uint32_t targetAddress;
		uint16_t currentPacketSize;
	} TURAG_PACKED;

	for (unsigned i = 0; i < packets; ++i) {
		uint16_t currentPacketSize = std::min(packetSize, byteAddress + length - targetAddress);
		

        reinterpret_cast<header_packed*>(request + myAddressLength + 1)->targetAddress = targetAddress;
        reinterpret_cast<header_packed*>(request + myAddressLength + 1)->currentPacketSize = currentPacketSize;

        uint8_t response[myAddressLength + 1 + currentPacketSize + 1];
		
        if (!transceive(request, sizeof(request), response, sizeof(response))) {
			return ErrorCode::transceive_error;
		}
		// this shouldn't happen
        if (response[myAddressLength] == TURAG_FELDBUS_BOOTLOADER_AVR_RESPONSE_FAIL_ADDRESS) {
			return ErrorCode::invalid_address;
		}
		
        memcpy(buffer, response + myAddressLength + 1, currentPacketSize);
		
		targetAddress += currentPacketSize;
		buffer += currentPacketSize;
	}
	return ErrorCode::success;
}

const char* BootloaderAvrBase::errorName(BootloaderAvrBase::ErrorCode errorCode) {
	switch (errorCode) {
		case ErrorCode::success: return "Erfolgreich.";
		case ErrorCode::invalid_size: return "Ungültige Größe.";
		case ErrorCode::invalid_address: return "Ungültige Adresse.";
		case ErrorCode::content_mismatch: return "Schreibvorgang fehlgeschlagen.";
		case ErrorCode::unsupported: return "Nicht unterstützt.";
		case ErrorCode::invalid_args: return "Ungültige Argumente.";
		case ErrorCode::transceive_error: return "Übertragung fehlgeschlagen.";
		case ErrorCode::preconditions_not_met: return "Voraussetzungen nicht erfüllt.";
	}
	return "";
}

const char* BootloaderAvrBase::errorDescription(BootloaderAvrBase::ErrorCode errorCode) {
	switch (errorCode) {
		case ErrorCode::success: return "Keine Fehler.";
		case ErrorCode::invalid_size: return "Das Gerät meldet, dass die vom Host angeforderte Datenmenge ungültig ist.";
		case ErrorCode::invalid_address: return "Das Gerät meldet, dass die vom Host angegebene Adresse ungültig ist.";
		case ErrorCode::content_mismatch: return "Das Gerät meldet, dass die geschriebenen Daten nicht den gelesenen entsprechen.";
		case ErrorCode::unsupported: return "Das Gerät meldet, dass die angeforderte Operation nicht unterstützt wird.";
		case ErrorCode::invalid_args: return "Einige der übergebenen Argumente sind nicht gültig.";
		case ErrorCode::transceive_error: return "Die Übertragung der Daten zum Gerät schlug fehl.";
		case ErrorCode::preconditions_not_met: return "Einige zum Ausführen dieses Befehls nötigen Kommandos scheiterten.";
	}
	return "";
}


bool BootloaderAtmega::readFuses(Fuses* fuseBuffer) {
	Device::Request<uint8_t> request;
	request.data = TURAG_FELDBUS_BOOTLOADER_ATMEGA_GET_FUSES;

	Device::Response<Fuses> response;

	if (!transceive(request, &response)) {
		return false;
	} else {
		*fuseBuffer = response.data;
		return true;
	}
}


bool BootloaderXmega::readFuses(Fuses* fuseBuffer) {
	Device::Request<uint8_t> request;
	request.data = TURAG_FELDBUS_BOOTLOADER_XMEGA_GET_FUSES;

	struct FusesRaw {
		uint8_t mask;
		uint8_t fuse0;
		uint8_t fuse1;
		uint8_t fuse2;
		uint8_t fuse3;
		uint8_t fuse4;
		uint8_t fuse5;
		uint8_t fuse6;
	} TURAG_PACKED;

	Device::Response<FusesRaw> response;

	if (!transceive(request, &response)) {
		return false;
	} else {
		if (response.data.mask & (1 << 0)) {
			fuseBuffer->fuse0.available = true;
			fuseBuffer->fuse0.value = response.data.fuse0;
		} else {
			fuseBuffer->fuse0.available = false;
		}

		if (response.data.mask & (1 << 1)) {
			fuseBuffer->fuse1.available = true;
			fuseBuffer->fuse1.value = response.data.fuse1;
		} else {
			fuseBuffer->fuse1.available = false;
		}

		if (response.data.mask & (1 << 2)) {
			fuseBuffer->fuse2.available = true;
			fuseBuffer->fuse2.value = response.data.fuse2;
		} else {
			fuseBuffer->fuse2.available = false;
		}

		if (response.data.mask & (1 << 3)) {
			fuseBuffer->fuse3.available = true;
			fuseBuffer->fuse3.value = response.data.fuse3;
		} else {
			fuseBuffer->fuse3.available = false;
		}

		if (response.data.mask & (1 << 4)) {
			fuseBuffer->fuse4.available = true;
			fuseBuffer->fuse4.value = response.data.fuse4;
		} else {
			fuseBuffer->fuse4.available = false;
		}

		if (response.data.mask & (1 << 5)) {
			fuseBuffer->fuse5.available = true;
			fuseBuffer->fuse5.value = response.data.fuse5;
		} else {
			fuseBuffer->fuse5.available = false;
		}

		if (response.data.mask & (1 << 6)) {
			fuseBuffer->fuse6.available = true;
			fuseBuffer->fuse6.value = response.data.fuse6;
		} else {
			fuseBuffer->fuse6.available = false;
		}

		return true;
	}
}

char BootloaderXmega::getRevisionId(void) {
	if (revisionId == 0) {
		Device::Request<uint8_t> request;
		request.data = TURAG_FELDBUS_BOOTLOADER_XMEGA_GET_REVISION;

		Device::Response<uint8_t> response;

		if (transceive(request, &response)) {
			revisionId = static_cast<char>(response.data + 65);
		}
	}
	return revisionId;
}

bool BootloaderStm32v2::readResetVectorStorageAddress() {
    struct BootloaderAddresses {
        uint32_t resetVectorStorageAddress;
    } TURAG_PACKED;

    Device::Request<uint8_t> request;
    request.data = TURAG_FELDBUS_BOOTLOADER_STM32V2_GET_APP_RESET_VECTOR_STORAGE_ADDRESS;

    Device::Response<BootloaderAddresses> response;

    if (transceive(request, &response)) {
        resetVectorStorageAddress = response.data.resetVectorStorageAddress;
        return true;
    }

    return false;
}

uint32_t BootloaderStm32v2::getResetVectorStorageAddress() {
    if (resetVectorStorageAddress == 0xFFFFFFFF) {
        readResetVectorStorageAddress();
    }
    return resetVectorStorageAddress;
}

TURAG::Feldbus::BootloaderAvrBase::ErrorCode BootloaderStm32v2::transmitAppResetVectors(uint32_t stackAddress, uint32_t resetHandlerAddress) {
    return transmitOrCommitAppResetVectors(TURAG_FELDBUS_BOOTLOADER_STM32V2_TRANSMIT_APP_RESET_VECTOR, stackAddress, resetHandlerAddress);
}

TURAG::Feldbus::BootloaderAvrBase::ErrorCode BootloaderStm32v2::commitAppResetVectors(uint32_t stackAddress, uint32_t resetHandlerAddress) {
    return transmitOrCommitAppResetVectors(TURAG_FELDBUS_BOOTLOADER_STM32V2_COMMIT_APP_RESET_VECTOR, stackAddress, resetHandlerAddress);
}


TURAG::Feldbus::BootloaderAvrBase::ErrorCode BootloaderStm32v2::transmitOrCommitAppResetVectors(uint8_t command, uint32_t stackAddress, uint32_t resetHandlerAddress) {
    struct StoreBootloaderAddressesRequest {
        uint8_t cmd;
        uint32_t stackAddress;
        uint32_t resethandlerAddress;
    } TURAG_PACKED;

    Device::Request<StoreBootloaderAddressesRequest> request;
    request.data.cmd = command;
    request.data.stackAddress = stackAddress;
    request.data.resethandlerAddress = resetHandlerAddress;

    Device::Response<uint8_t> response;

    if (transceive(request, &response)) {
        return static_cast<TURAG::Feldbus::BootloaderAvrBase::ErrorCode>(response.data);
    }

    return TURAG::Feldbus::BootloaderAvrBase::ErrorCode::transceive_error;
}

const char* Bootloader::getMcuName(uint16_t mcuId) {
    switch(mcuId) {
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_ATMEGA8:   return "ATmega8";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_ATMEGA16:  return "ATmega16";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_ATMEGA32:  return "ATmega32";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_ATMEGA128: return "ATmega128";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_ATMEGA88:  return "ATmega88";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_ATMEGA168: return "ATmega168";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_ATMEGA644: return "ATmega644";

    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_XMEGA16D4: return "ATxmega16D4";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_XMEGA32D4: return "ATxmega32D4";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_XMEGA64D4: return "ATxmega64D4";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_XMEGA128D4:return "ATxmega128D4";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_XMEGA16E5: return "ATxmega16E5";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_XMEGA32E5: return "ATxmega32E5";

    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_STM32F051x8: return "STM32F051x8";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_STM32F030x4: return "STM32F030x4";
    case TURAG_FELDBUS_BOOTLOADER_MCU_ID_STM32F031x6: return "STM32F031x6";
    default: return "<unknown mcu id>";
    }
}

} // namespace Feldbus
} // namespace TURAG

#endif // TURAG_USE_TURAG_FELDBUS_HOST
