/**
 *  @brief		TURAG feldbus actor base class
 *  @file		aktor_tina.cpp
 *  @date		22.03.2013
 *  @author		Martin Oemus
 *
 */

/* We are not doing anything special with float values, but we need to check
 * whether a passed value (factor) is equal to a special value.
 * See TURAG_FELDBUS_STELLANTRIEBE_COMMAND_FACTOR_CONTROL_VALUE */
#pragma GCC diagnostic ignored "-Wfloat-equal"

#define TURAG_DEBUG_LOG_SOURCE "B"

#include <tina++/tina.h>

#if TURAG_USE_TURAG_FELDBUS_HOST


#include "legacystellantriebedevice.h"

#include <tina/debug/print.h>

#include <cstring>


namespace TURAG {
namespace Feldbus {

struct AktorGetShort {
    int16_t value;
} TURAG_PACKED;

struct AktorGetLong {
    int32_t value;
} TURAG_PACKED;

struct AktorGetFloat {
    float value;
} TURAG_PACKED;

struct AktorSetChar {
    uint8_t key;
    int8_t value;
} TURAG_PACKED;

struct AktorSetShort {
    uint8_t key;
    int16_t value;
} TURAG_PACKED;

struct AktorSetLong {
    uint8_t key;
    int32_t value;
} TURAG_PACKED;

struct AktorSetFloat {
    uint8_t key;
    float value;
} TURAG_PACKED;

struct AktorGetCommandInfo {
    uint8_t key;
    uint8_t cmd0;
    uint8_t cmd1;
    uint8_t cmd2;
} TURAG_PACKED;

struct AktorGetCommandInfoResponse {
    LegacyStellantriebeDevice::Command_t::WriteAccess writeAccess;
    LegacyStellantriebeDevice::Command_t::CommandLength length;
    float factor;
} TURAG_PACKED;

struct AktorGetStructuredOutputControl {
    uint8_t key;
    uint8_t cmd;
} TURAG_PACKED;

unsigned int LegacyStellantriebeDevice::getCommandsetLength(void) {
    if (commandSetPopulated) {
        return commandSetLength;
    } else {
        Request<AktorGetCommandInfo> request;
        request.data.key = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET_COMMANDSET_SIZE;
        request.data.cmd0 = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET_COMMANDSET_SIZE;
        request.data.cmd1 = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET_COMMANDSET_SIZE;
        request.data.cmd2 = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET_COMMANDSET_SIZE;

        Response<uint8_t> response;
        if (!transceive(request, &response)) {
            return 0;
        }
        return response.data;
    }
}

bool LegacyStellantriebeDevice::populateCommandSet(Command_t* commandSet_, unsigned int commandSetLength_) {
    if (!commandSet_) return false;
    
    unsigned int deviceCommandSetLength = getCommandsetLength();
    if (deviceCommandSetLength == 0) return false;
    
    if (deviceCommandSetLength > commandSetLength_) {
        commandSetLength = commandSetLength_;
		// Because it is not unlikely that the user supplied a shorter buffer on purpose, 
		// this message has debug level.
        turag_debugf("%s: commandset will be truncated (buffer requires space for %d more elements)", name(), deviceCommandSetLength - commandSetLength_);
    } else {
        commandSetLength = deviceCommandSetLength;
    }
    commandSet = commandSet_;
    
    Request<AktorGetCommandInfo> request;
    request.data.key = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET;
    request.data.cmd0 = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET;
    request.data.cmd1 = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET;
    request.data.cmd2 = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET;
    Response<AktorGetCommandInfoResponse> response;
    
    for (unsigned int i = 0; i < commandSetLength; ++i) {
		request.data.key = static_cast<uint8_t>(i + 1);

        if (!transceive(request, &response)) {
            return false;
        }

        // filter unknown values for the data type/length
        Command_t::CommandLength data_type = static_cast<Command_t::CommandLength>(response.data.length);
        if (data_type != Command_t::CommandLength::none &&
                data_type != Command_t::CommandLength::text &&
                data_type != Command_t::CommandLength::length_char &&
                data_type != Command_t::CommandLength::length_short &&
                data_type != Command_t::CommandLength::length_long &&
                data_type != Command_t::CommandLength::length_float) {
            data_type = Command_t::CommandLength::none;
    }

        // filter unkown values for access mode
        Command_t::WriteAccess access_mode = static_cast<Command_t::WriteAccess>(response.data.writeAccess);
        if (access_mode != Command_t::WriteAccess::read_only &&
                access_mode != Command_t::WriteAccess::write_only &&
                access_mode != Command_t::WriteAccess::read_write) {
            access_mode = Command_t::WriteAccess::read_only;
        }

        commandSet[i].writeAccess = access_mode;
        commandSet[i].length = data_type;
        commandSet[i].factor = response.data.factor;
    }
    commandSetPopulated = true;
    return true;
}

bool LegacyStellantriebeDevice::getValue(uint8_t key, float* value) {
    if (!commandSetPopulated) {
        turag_errorf("%s: commandSet not populated", name());
        return false;
    }
    if (key > commandSetLength || key == 0) {
        turag_errorf("%s: key not within commandSetLength", name());
        return false;
    }

    Command_t* command = &commandSet[key-1];
    
    if (command->length == Command_t::CommandLength::none || command->length == Command_t::CommandLength::text) {
        turag_errorf("%s: key not supported", name());
        return false;
    }
    if (command->factor == TURAG_FELDBUS_STELLANTRIEBE_COMMAND_FACTOR_CONTROL_VALUE &&
            command->length != Command_t::CommandLength::length_float) {
		turag_errorf("%s: value with key %u is not floating point, which was requested", name(), key);
        return false;
    }
    
    if (!command->bufferValid) {
        Request<uint8_t> request;
        request.data = key;

        switch (command->length) {
        case Command_t::CommandLength::length_char: {
            Response<int8_t> response;
            if (!transceive(request, &response)) {
                return false;
            }
            command->buffer.floatValue = static_cast<float>(response.data) * command->factor;
            break;
        }
        case Command_t::CommandLength::length_short: {
            Response<AktorGetShort> response;
            if (!transceive(request, &response)) {
                return false;
            }
            command->buffer.floatValue = static_cast<float>(response.data.value) * command->factor;
            break;
        }
        case Command_t::CommandLength::length_long: {
            Response<AktorGetLong> response;
            if (!transceive(request, &response)) {
                return false;
            }
            command->buffer.floatValue = static_cast<float>(response.data.value) * command->factor;
            break;
        }
        default: {
            Response<AktorGetFloat> response;
            if (!transceive(request, &response)) {
                return false;
            }
            if (command->factor != TURAG_FELDBUS_STELLANTRIEBE_COMMAND_FACTOR_CONTROL_VALUE) {
                command->buffer.floatValue = response.data.value * command->factor;
            } else {
                command->buffer.floatValue = response.data.value;
            }
            break;
        }
        }

        // protocol definition: write-only values are bufferable
        if (command->writeAccess == Command_t::WriteAccess::write_only) {
            command->bufferValid = true;
        }

    }
    *value = command->buffer.floatValue;
    return true;
}

bool LegacyStellantriebeDevice::getValue(uint8_t key, int32_t* value) {
    if (!commandSetPopulated) {
        turag_errorf("%s: commandSet not populated", name());
        return false;
    }
    if (key > commandSetLength || key == 0) {
        turag_errorf("%s: key not within commandSetLength", name());
        return false;
    }

    Command_t* command = &commandSet[key-1];

    if (command->length == Command_t::CommandLength::none || command->length == Command_t::CommandLength::text) {
        turag_errorf("%s: key not supported", name());
        return false;
    }
    if (command->factor != TURAG_FELDBUS_STELLANTRIEBE_COMMAND_FACTOR_CONTROL_VALUE ||
            command->length == Command_t::CommandLength::length_float) {
        turag_errorf("%s: value is floating point", name());
        return false;
    }

    if (!command->bufferValid) {
        Request<uint8_t> request;
        request.data = key;

        switch (command->length) {
        case Command_t::CommandLength::length_char: {
            Response<int8_t> response;
            if (!transceive(request, &response)) {
                return false;
            }
            command->buffer.intValue = response.data;
            break;
        }
        case Command_t::CommandLength::length_short: {
            Response<AktorGetShort> response;
            if (!transceive(request, &response)) {
                return false;
            }
            command->buffer.intValue = response.data.value;
            break;
        }
        case Command_t::CommandLength::length_long: {
            Response<AktorGetLong> response;
            if (!transceive(request, &response)) {
                return false;
            }
            command->buffer.intValue = response.data.value;
            break;
        }
        case Command_t::CommandLength::length_float: {
            Response<AktorGetFloat> response;
            if (!transceive(request, &response)) {
                return false;
            }
            command->buffer.intValue = response.data.value;
            break;
        }
        default:
            break;
        }

        // protocol definition: write-only values are bufferable
        if (command->writeAccess == Command_t::WriteAccess::write_only) {
            command->bufferValid = true;
        }

    }
    *value = command->buffer.intValue;
    return true;
}


bool LegacyStellantriebeDevice::setValue(uint8_t key, float value) {
    if (!commandSetPopulated) {
        turag_errorf("%s: commandSet not populated", name());
        return false;
    }
    if (key > commandSetLength || key == 0) {
        turag_errorf("%s: key not within commandSetLength", name());
        return false;
    }

    Command_t* command = &commandSet[key-1];

    if (command->length == Command_t::CommandLength::none || command->length == Command_t::CommandLength::text) {
        turag_errorf("%s: key not supported", name());
        return false;
    }
    if (command->writeAccess == Command_t::WriteAccess::read_only) {
        turag_errorf("%s: key not writable", name());
        return false;
    }
    if (command->factor == TURAG_FELDBUS_STELLANTRIEBE_COMMAND_FACTOR_CONTROL_VALUE &&
            command->length != Command_t::CommandLength::length_float) {
		turag_errorf("%s: value with key %u is not floating point, but float shall be set", name(), key);
        return false;
    }

    command->bufferValid = false;
    Response<> response;

    switch (command->length) {
    case Command_t::CommandLength::length_char: {
        Request<AktorSetChar> request;
        request.data.key = key;
        request.data.value = static_cast<int8_t>(value / command->factor);

        if (!transceive(request, &response)) {
            return false;
        }
        break;
    }
    case Command_t::CommandLength::length_short: {
        Request<AktorSetShort> request;
        request.data.key = key;
        request.data.value = static_cast<int16_t>(value / command->factor);

        if (!transceive(request, &response)) {
            return false;
        }
        break;
    }
    case Command_t::CommandLength::length_long: {
        Request<AktorSetLong> request;
        request.data.key = key;
        request.data.value = static_cast<int32_t>(value / command->factor);

        if (!transceive(request, &response)) {
            return false;
        }
        break;
    }
    default: {
        Request<AktorSetFloat> request;
        request.data.key = key;

        if (command->factor != TURAG_FELDBUS_STELLANTRIEBE_COMMAND_FACTOR_CONTROL_VALUE) {
            request.data.value = value / command->factor;
        } else {
            request.data.value = value;
        }

        if (!transceive(request, &response)) {
            return false;
        }
        break;
    }
    }
    return true;
}

bool LegacyStellantriebeDevice::setValue(uint8_t key, int32_t value) {
    if (!commandSetPopulated) {
        turag_errorf("%s: commandSet not populated", name());
        return false;
    }
    if (key > commandSetLength || key == 0) {
        turag_errorf("%s: key not within commandSetLength", name());
        return false;
    }

    Command_t* command = &commandSet[key-1];

    if (command->length == Command_t::CommandLength::none || command->length == Command_t::CommandLength::text) {
        turag_errorf("%s: key not supported", name());
        return false;
    }
    if (command->writeAccess == Command_t::WriteAccess::read_only) {
        turag_errorf("%s: key not writable", name());
        return false;
    }
    if (command->factor != TURAG_FELDBUS_STELLANTRIEBE_COMMAND_FACTOR_CONTROL_VALUE ||
            command->length == Command_t::CommandLength::length_float) {
        turag_errorf("%s: value is floating point", name());
        return false;
    }

    command->bufferValid = false;
    Response<> response;

    switch (command->length) {
    case Command_t::CommandLength::length_char: {
        Request<AktorSetChar> request;
        request.data.key = key;
        request.data.value = static_cast<int8_t>(value);

        if (!transceive(request, &response)) {
            return false;
        }
        break;
    }
    case Command_t::CommandLength::length_short: {
        Request<AktorSetShort> request;
        request.data.key = key;
        request.data.value = static_cast<int16_t>(value);

        if (!transceive(request, &response)) {
            return false;
        }
        break;
    }
    case Command_t::CommandLength::length_long: {
        Request<AktorSetLong> request;
        request.data.key = key;
        request.data.value = static_cast<int32_t>(value);

        if (!transceive(request, &response)) {
            return false;
        }
        break;
    }
    case Command_t::CommandLength::length_float: {
        Request<AktorSetLong> request;
        request.data.key = key;
        request.data.value = static_cast<int32_t>(value);

        if (!transceive(request, &response)) {
            return false;
        }
        break;
    }
    default:
        break;
    }
    return true;
}

unsigned int LegacyStellantriebeDevice::getCommandNameLength(uint8_t key) {
    if (!commandSetPopulated) {
        turag_errorf("%s: commandSet not populated", name());
        return false;
    }
    if (key > commandSetLength || key == 0) {
        turag_errorf("%s: key not within commandSetLength", name());
        return false;
    }

    Command_t* command = &commandSet[key-1];

    if (command->length == Command_t::CommandLength::none) {
        turag_errorf("%s: key not supported", name());
        return false;
    }

    Request<AktorGetCommandInfo> request;
    request.data.key = key;
    request.data.cmd0 = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET_NAME_LENGTH;
    request.data.cmd1 = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET_NAME_LENGTH;
    request.data.cmd2 = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET_NAME_LENGTH;

    Response<uint8_t> response;

    if (!transceive(request, &response)) {
        return 0;
    } else {
        return response.data;
    }
}

bool LegacyStellantriebeDevice::getCommandName(uint8_t key, char* out_name) {
    if (!out_name) {
        return false;
    }

	unsigned name_length = getCommandNameLength(key);
    if (name_length == 0) {
        *out_name = 0;
        return false;
    }
    
    uint8_t request[myAddressLength + 4 + 1];
    request[myAddressLength] = key;
    request[myAddressLength + 1] = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET_NAME;
    request[myAddressLength + 2] = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET_NAME;
    request[myAddressLength + 3] = TURAG_FELDBUS_STELLANTRIEBE_COMMAND_INFO_GET_NAME;
    
    if (!transceive(request,
                    sizeof(request),
                    reinterpret_cast<uint8_t*>(out_name),
                    name_length + myAddressLength + 1)) {
        return false;
    }

	for (unsigned i = 0; i < name_length; ++i) {
        out_name[i] = out_name[i + myAddressLength];
    }
    out_name[name_length] = 0;

    return true;

}

#if TURAG_FELDBUS_AKTOR_STRUCTURED_OUTPUT_AVAILABLE

int LegacyStellantriebeDevice::getStructuredOutputTableLength(void) {
    if (structuredOutputTableLength != -1) {
        return structuredOutputTableLength;
    } else {
        Request<AktorGetStructuredOutputControl> request;
        request.data.key = TURAG_FELDBUS_STELLANTRIEBE_STRUCTURED_OUTPUT_CONTROL;
        request.data.cmd = TURAG_FELDBUS_STELLANTRIEBE_STRUCTURED_OUTPUT_GET_BUFFER_SIZE;

        Response<uint8_t> response;

        if (!transceive(request, &response)) {
            return -1;
        }
		structuredOutputTableLength = static_cast<int>(response.data);
        return structuredOutputTableLength;
    }
}


bool LegacyStellantriebeDevice::setStructuredOutputTable(const std::vector<uint8_t>& keys) {
	if (static_cast<int>(keys.size()) > getStructuredOutputTableLength()) {
		turag_errorf("%s: output table in device too small for number of provided keys", name());
        return false;
    }
    if (!commandSetPopulated) {
		turag_errorf("%s: commandSet not populated", name());
        return false;
    }
    
    for (unsigned int i = 0; i < keys.size(); ++i) {
        if (keys[i] > getCommandsetLength() || commandSet[keys[i] - 1].length == Command_t::CommandLength::none) {
			turag_errorf("%s: requested keys invalid", name());
            return false;
        }
    }
    
    uint8_t request[keys.size() + myAddressLength + 3];

    request[myAddressLength + 0] = TURAG_FELDBUS_STELLANTRIEBE_STRUCTURED_OUTPUT_CONTROL;
    request[myAddressLength + 1] = TURAG_FELDBUS_STELLANTRIEBE_STRUCTURED_OUTPUT_SET_STRUCTURE;
    memcpy(request + myAddressLength + 2, keys.data(), keys.size());
    
    uint8_t response[myAddressLength + 1 + 1];
    
    if (transceive(
			request, 
			sizeof(request),
            response,
			sizeof(response))) {
        if (response[myAddressLength] == TURAG_FELDBUS_STELLANTRIEBE_STRUCTURED_OUTPUT_TABLE_OK) {
            structuredOutputTable = keys;
            return true;
        }
    }
    
    return false;
}


bool LegacyStellantriebeDevice::getStructuredOutput(std::vector<StructuredDataPair_t>* values) {
    if (structuredOutputTable.size() == 0) {
		turag_errorf("%s: structured output mapping empty", name());
        return false;
    }
    if (!commandSetPopulated) {
		turag_errorf("%s: commandSet not populated", name());
        return false;
    }
    
	unsigned data_size = 0;
    for (unsigned int i = 0; i < structuredOutputTable.size(); ++i) {
        switch (commandSet[structuredOutputTable[i] - 1].length) {
        case Command_t::CommandLength::length_char:
            data_size += 1;
            break;
        case Command_t::CommandLength::length_short:
            data_size += 2;
            break;
        case Command_t::CommandLength::length_long:
            data_size += 4;
            break;
        case Command_t::CommandLength::length_float:
            data_size += 4;
            break;
        default:
            return false;
        }
    }

    uint8_t request[myAddressLength + 1 + 1];
    request[myAddressLength] = TURAG_FELDBUS_STELLANTRIEBE_STRUCTURED_OUTPUT_GET;

    uint8_t* response = new uint8_t[myAddressLength + data_size + 1];

    if (transceive(request,
                   sizeof(request),
                   response,
                   myAddressLength + data_size + 1)) {
        uint8_t* pValue = response + myAddressLength;

        for (unsigned int i = 0; i < structuredOutputTable.size(); ++i) {
            int32_t device_value;

            switch (commandSet[structuredOutputTable[i] - 1].length) {
            case Command_t::CommandLength::length_char:
                device_value = *(reinterpret_cast<int8_t*>(pValue));
                pValue += 1;
                break;
            case Command_t::CommandLength::length_short:
                device_value = *(reinterpret_cast<int16_t*>(pValue));
                pValue += 2;
                break;
            case Command_t::CommandLength::length_long:
                device_value = *(reinterpret_cast<int32_t *>(pValue));
                pValue += 4;
                break;
            case Command_t::CommandLength::length_float:
                device_value = *(reinterpret_cast<float *>(pValue));
                pValue += 4;
                break;
            default:
                return false;
            }

            StructuredDataPair_t pair;
            pair.key = structuredOutputTable[i];

            if (commandSet[structuredOutputTable[i] - 1].factor == TURAG_FELDBUS_STELLANTRIEBE_COMMAND_FACTOR_CONTROL_VALUE) {
                pair.value = static_cast<float>(device_value);
            } else {
                pair.value = static_cast<float>(device_value) * commandSet[structuredOutputTable[i] - 1].factor;
            }
            values->push_back(pair);
        }
        delete[] response;
        return true;
    }

    delete[] response;
    return false;
}

#endif // TURAG_FELDBUS_AKTOR_STRUCTURED_OUTPUT_AVAILABLE

} // namespace Feldbus
} // namespace TURAG

#endif // TURAG_USE_TURAG_FELDBUS_HOST
