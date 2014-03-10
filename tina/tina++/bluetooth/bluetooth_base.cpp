
#define TURAG_DEBUG_LOG_SOURCE "Y"

#include <tina++/debug.h>

#include "bluetooth_base.h"
#include <tina++/tina.h>
#include <tina++/container/thread_fifo.h>
#include <tina++/utils/base64.h>
#include <cstring>
#include <array>
#include <algorithm>

namespace TURAG {


BluetoothBase* BluetoothBase::instance_ = nullptr;


// we need this dirty function because we can't start a thread
// with a member function pointer.
void BluetoothBase::thread_entry(void) {
    if (instance_) {
        instance_->main_thread_func();
    }
}

void BluetoothBase::init(void) {
    for (int i = 0; i < BLUETOOTH_NUMBER_OF_PEERS; ++i) {
        peersEnabled[i].store(false, std::memory_order_relaxed);
    }

    lowlevelInit();

    instance_ = this;
    bluetooth_main_thread.start(BLUETOOTH_THREAD_PRIORITY, thread_entry);
}

void BluetoothBase::main_thread_func(void) {
    Thread<>::setName("bluetooth-high");

    info("Bluetooth-High-Level thread started");

    Rpc_t rpc;

    while(!bluetooth_main_thread.shouldTerminate()) {
        if (rpc_fifo.fetch(&rpc, ms_to_ticks(BLUETOOTH_THREAD_RPC_WAIT_MS))) {
            if (rpc.received) {
                Mutex::Lock lock(rpc_mutex);
                RpcFunction callback = rpc_functions[rpc.data.rpc_id];
                lock.unlock();

                if (callback) {
                    debugf("call RPC %d", rpc.data.rpc_id);
                    (*callback)(rpc.peer_id, rpc.data.param);
                }
            } else {
                int buffer_size = Base64::encodeLength(rpc.data) + 2;
                uint8_t buffer[buffer_size];
                // start byte
                buffer[0] = 2;
                // data
                Base64::encode(rpc.data, buffer + 1);
                // end byte
                buffer[buffer_size - 1] = 3;
                if (getConnectionStatusLowlevel(rpc.peer_id) == Status::connected && write(rpc.peer_id, buffer, buffer_size)) {
                    debugf("call Remote-RPC %d on peer %d", rpc.data.rpc_id, rpc.peer_id);
                } else {
                    warningf("call Remote-RPC %d on peer %d - FAILED", rpc.data.rpc_id, rpc.peer_id);
                }
            }
        }

        // check DataProvider
        for (DataProvider& provider: data_providers) {
            Mutex::Lock lock(data_provider_mutex);
            if (provider.sendRequest) {
                int encoded_buffer_size = Base64::encodeLength(provider.buffer_size) + 2;
                uint8_t buffer[encoded_buffer_size];
                // start byte
                buffer[0] = 2;
                // ID
                provider.buffer[0] = provider.id + 64;
                // data
                Base64::encode(provider.buffer, provider.buffer_size, buffer + 1);
                // end byte
                buffer[encoded_buffer_size - 1] = 3;
                uint8_t dest = provider.destination;
                uint8_t id = provider.id;
                provider.sendRequest = false;
                lock.unlock();
                // write to bluetooth, we unlock the mutex in case the writing takes a bit more time
                if (getConnectionStatusLowlevel(dest) == Status::connected && write(dest, buffer, encoded_buffer_size)) {
                    debugf("DataProvider %d push to %d", id, dest);
                } else {
                    warningf("DataProvider %d push to %d - FAILED", id, dest);
                }
            }
        }
    }

}

void BluetoothBase::highlevelParseIncomingData(uint8_t sender, uint8_t* buffer, size_t buffer_size) {
    if (sender >= BLUETOOTH_NUMBER_OF_PEERS) return;

    for (unsigned i = 0; i < buffer_size; ++i) {
        if (buffer[i] == 2) {
            in_buffer[sender].index = 0;
            in_buffer[sender].waitForStartByte = false;
        } else if (in_buffer[sender].waitForStartByte) {
            continue;
        } else if (buffer[i] == 3) {
            uint8_t decode_buffer[Base64::decodeLength(in_buffer[sender].index)];
            size_t decoded_size = Base64::decode(in_buffer[sender].data, in_buffer[sender].index, decode_buffer);

            if (decode_buffer[0] < 64) {
                // RPC
                if (decode_buffer[0] >= BLUETOOTH_NUMBER_OF_RPCS) {
                    criticalf("RPC %d recv from %d--> ID invalid", decode_buffer[0], sender);
                } else if (decoded_size != sizeof(RpcData)) {
                    criticalf("RPC %d recv from %d --> package size mismatch (%d)", decode_buffer[0], sender, static_cast<int>(decoded_size));
                } else {
                    Rpc_t rpc;
                    std::memcpy(&rpc.data, decode_buffer, sizeof(RpcData));
                    rpc.peer_id = sender;
                    rpc.received = true;
                    rpc_fifo.post(rpc);
                    debugf("RPC %d recv from %d --> queued for exec", decode_buffer[0], sender);
                }
            } else {
                // data for DataSink
                uint8_t data_sink_id = decode_buffer[0] - 64;
                if (data_sink_id < BLUETOOTH_NUMBER_OF_DATA_SINKS) {
                    Mutex::Lock lock(data_sink_mutex);
                    if (data_sinks[data_sink_id].buffer && decoded_size == data_sinks[data_sink_id].buffer_size && decoded_size > 1) {
                        debugf("DataSink %d recv data from %d", data_sink_id, sender);
                        std::memcpy(data_sinks[data_sink_id].buffer, &decode_buffer, decoded_size);
                    }
                } else {
                    criticalf("DataSink %d recv data from %d -> ID invalid!", data_sink_id, sender);
                }
            }
        } else {
            in_buffer[sender].data[in_buffer[sender].index] = buffer[i];
            ++in_buffer[sender].index;
            if (in_buffer[sender].index == sizeof(InBuffer::data)) {
                in_buffer[sender].waitForStartByte = true;
                warningf("Overflow in recv-buffer for peer %d", sender);
            }
        }
    }
}



bool BluetoothBase::registerRpcFunction(uint8_t rpc_id, RpcFunction callback) {
    if (rpc_id < BLUETOOTH_NUMBER_OF_RPCS) {
        Mutex::Lock lock(rpc_mutex);
        rpc_functions[rpc_id] = callback;
        return true;
    } else {
        error("invalid RPC ID");
        return false;
    }
}


bool BluetoothBase::callRpc(uint8_t destination, uint8_t rpc_id, uint64_t param) {
    if (destination >= BLUETOOTH_NUMBER_OF_PEERS) {
        error("invalid peer ID");
        return false;
    } else if (!peersEnabled[destination].load(std::memory_order_relaxed)) {
        return false;
    } else {
        Rpc_t rpc;
        rpc.data.rpc_id = rpc_id;
        rpc.data.param = param;
        rpc.received = false;
        rpc.peer_id = destination;
        rpc_fifo.post(rpc);
        return true;
    }
}

bool BluetoothBase::addDataSink(uint8_t data_sink_id, uint8_t* storage_buffer, size_t length) {
    if (data_sink_id >= BLUETOOTH_NUMBER_OF_DATA_SINKS || !storage_buffer) {
        error("invalid DataSink ID or storage pointer zero");
        return false;
    } else if (length > 92 + 1) {
		error("DataSinks can not hold data bigger than 92 byte");
		return false;
    } else {
        Mutex::Lock lock(data_sink_mutex);
        if (data_sinks[data_sink_id].buffer) {
            warningf("datasink %d was overwritten", data_sink_id);
        }
        data_sinks[data_sink_id] = DataSink(storage_buffer, length);
        return true;
    }
}

bool BluetoothBase::getData(uint8_t data_sink_id, uint8_t* buffer, size_t length) {
    if (data_sink_id >= BLUETOOTH_NUMBER_OF_DATA_SINKS || !buffer) {
        error("invalid DataSink ID or buffer pointer zero");
        return false;
    } else {
        Mutex::Lock lock(data_sink_mutex);
        if (length != data_sinks[data_sink_id].buffer_size) {
            errorf("incorrect data length for DataSink %d", data_sink_id);
            return false;
        }
        std::memcpy(buffer, data_sinks[data_sink_id].buffer, length);
        return true;
    }
}


bool BluetoothBase::addDataProvider(uint8_t destination, uint8_t data_provider_id, uint8_t* storage_buffer, size_t length) {
    if (!storage_buffer) {
        error("storage pointer zero");
        return false;
    } else if (length > 92 + 1) {
        error("DataProvider can not hold data bigger than 92 byte");
        return false;
    } else if (destination > BLUETOOTH_NUMBER_OF_PEERS) {
        error("DataSinks invalid peer ID");
        return false;
    } else {
        Mutex::Lock lock(data_provider_mutex);
        ArrayBuffer<DataProvider, BLUETOOTH_NUMBER_OF_DATA_PROVIDERS>::iterator provider = std::find_if(data_providers.begin(), data_providers.end(), [&] (const DataProvider& provider) { return provider.destination == destination && provider.id == data_provider_id; });
        if (provider != data_providers.end()) {
            warningf("dataprovider %d was overwritten", data_provider_id);
            provider->buffer = storage_buffer;
            provider->buffer_size = length;
            provider->destination = destination;
            provider->id = data_provider_id;
        } else {
            data_providers.emplace_back(DataProvider(storage_buffer, length, destination, data_provider_id));
        }
        return true;
    }
}

bool BluetoothBase::pushData(uint8_t destination, uint8_t data_provider_id, const uint8_t* data, size_t length) {
    ArrayBuffer<DataProvider, BLUETOOTH_NUMBER_OF_DATA_PROVIDERS>::iterator provider = std::find_if(data_providers.begin(), data_providers.end(), [&] (const DataProvider& provider) { return provider.destination == destination && provider.id == data_provider_id; });
    if (provider == data_providers.end()) {
        error("specified data provider couldn't be found");
        return false;
    } else if (!peersEnabled[destination].load(std::memory_order_relaxed)) {
        return false;
    } else {
        Mutex::Lock lock(data_provider_mutex);
        if (length != provider->buffer_size) {
            errorf("incorrect data length (pushData dest=%d id=%d)", destination, data_provider_id);
            return false;
        }
        std::memcpy(provider->buffer, data, length);
        provider->sendRequest = true;
        return true;
    }
}


bool BluetoothBase::pushData(uint8_t destination, uint8_t data_provider_id) {
    ArrayBuffer<DataProvider, BLUETOOTH_NUMBER_OF_DATA_PROVIDERS>::iterator provider = std::find_if(data_providers.begin(), data_providers.end(), [&] (const DataProvider& provider) { return provider.destination == destination && provider.id == data_provider_id; });
    if (provider == data_providers.end()) {
        error("specified data provider couldn't be found");
        return false;
    } else if (!peersEnabled[destination].load(std::memory_order_relaxed)) {
        return false;
    } else {
        Mutex::Lock lock(data_provider_mutex);
        provider->sendRequest = true;
        return true;
    }
}


BluetoothBase::Status BluetoothBase::getConnectionStatus(uint8_t peer_id) {
    if (peer_id >= BLUETOOTH_NUMBER_OF_PEERS) return Status::disconnected;

    if (peersEnabled[peer_id].load(std::memory_order_relaxed)) {
        return getConnectionStatusLowlevel(peer_id);
    } else {
        return Status::disconnected;
    }
}


void BluetoothBase::setPeerEnabled(uint8_t peer_id, bool enabled) {
    if (peer_id >= BLUETOOTH_NUMBER_OF_PEERS) return;

    peersEnabled[peer_id].store(enabled, std::memory_order_relaxed);
    setPeerEnabledLowlevel(peer_id, enabled);
}



} // namespace TURAG
