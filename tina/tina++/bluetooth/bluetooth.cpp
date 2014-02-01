
#include "bluetooth.h" 
#include <tina/bluetooth_config.h>
#include <tina++/thread.h>
#include <tina++/tina.h>
#include <tina++/container/thread_fifo.h>
#include <tina++/utils/base64.h>
#include <cstring>

namespace TURAG {
namespace Bluetooth {

struct RpcData {
    uint8_t rpc_id;
    uint64_t param;
} _packed;

struct Rpc_t {
    RpcData data;
    bool received;
    uint8_t peer_id;
} _packed;

struct InBuffer {
    uint8_t index;
    bool waitForStartByte;
    uint8_t data[128];

    InBuffer() :
        index(0),
        waitForStartByte(false) { }
};

struct DataProvider {
    uint8_t* buffer;
    size_t buffer_size;
    bool sendRequest;
    uint8_t destination;

    DataProvider() :
        buffer(nullptr),
        buffer_size(0),
        sendRequest(false),
        destination(0) { }

    DataProvider(uint8_t* buf, size_t buf_size, uint8_t dest) :
        buffer(buf),
        buffer_size(buf_size),
        sendRequest(false),
        destination(dest) { }
};

struct DataSink {
    uint8_t* buffer;
    size_t buffer_size;
    bool newDataAvailable;
    uint8_t lastSourceId;

    DataSink() :
        buffer(nullptr),
        buffer_size(0),
        newDataAvailable(false),
        lastSourceId(0) { }

    DataSink(uint8_t* buf, size_t buf_size) :
        buffer(buf),
        buffer_size(buf_size),
        newDataAvailable(false),
        lastSourceId(0) { }
};

static Thread<BLUETOOTH_SEND_THREAD_STACK_SIZE> main_thread;
static RpcFunction rpc_functions[BLUETOOTH_NUMBER_OF_RPCS] = {0};
static Mutex rpc_mutex;
static DataProvider data_providers[BLUETOOTH_NUMBER_OF_DATA_PROVIDERS];
static Mutex data_provider_mutex;
static DataSink data_sinks[BLUETOOTH_NUMBER_OF_DATA_SINKS];
static Mutex data_sink_mutex;
TURAG_THREAD_ENTRY static void main_thread_func(void);

static ThreadFifo<Rpc_t, 32> rpc_fifo;

static InBuffer in_buffer[BLUETOOTH_NUMBER_OF_PEERS];




void init(void) {
    lowlevelInit();

    main_thread.start(BLUETOOTH_SEND_THREAD_PRIORITY, main_thread_func);
}

TURAG_THREAD_ENTRY static void main_thread_func(void) {
    Rpc_t rpc;

    while(1) {
        if (rpc_fifo.fetch(&rpc, ms_to_ticks(BLUETOOTH_SEND_THREAD_RPC_WAIT_MS))) {
            if (rpc.received) {
                Mutex::Lock lock(rpc_mutex);
                RpcFunction callback = rpc_functions[rpc.data.rpc_id];
                lock.unlock();

                if (callback) {
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
                write(rpc.peer_id, buffer, buffer_size);
            }
        }

        // check DataProvider
        Mutex::Lock lock(data_provider_mutex);
        for (int i = 0; i < BLUETOOTH_NUMBER_OF_DATA_PROVIDERS; ++i) {
            lock.lock();
            if (data_providers[i].buffer && data_providers[i].sendRequest) {
                int encoded_buffer_size = Base64::encodeLength(data_providers[i].buffer_size) + 3;
                uint8_t buffer[encoded_buffer_size];
                // start byte
                buffer[0] = 2;
                // ID
                buffer[1] = i + 64;
                // data
                Base64::encode(data_providers[i].buffer, data_providers[i].buffer_size, buffer + 2);
                // end byte
                buffer[encoded_buffer_size - 1] = 3;
                uint8_t dest = data_providers[i].destination;
                data_providers[i].sendRequest = false;
                lock.unlock();
                // write to bluetooth
                write(dest, buffer, encoded_buffer_size);
            }
        }
    }

}


void highlevelParseIncomingData(uint8_t sender, uint8_t* buffer, size_t buffer_size) {
    if (sender >= BLUETOOTH_NUMBER_OF_PEERS) return;

    for (unsigned i = 0; i < buffer_size; ++i) {
        if (buffer[i] == 2) {
            in_buffer[sender].index = 0;
            in_buffer[sender].waitForStartByte = false;
        } else if (in_buffer[sender].waitForStartByte) {
            continue;
        } else if (buffer[i] == 3) {
            uint8_t decode_buffer[Base64::decodeLength(in_buffer[sender].index)];
            size_t size = Base64::decode(in_buffer[sender].data, in_buffer[sender].index, decode_buffer);

            if (decode_buffer[0] < 64) {
                // RPC
                if (decode_buffer[0] < BLUETOOTH_NUMBER_OF_RPCS && size == sizeof(RpcData)) {
                    Rpc_t rpc;
                    std::memcpy(&rpc.data, decode_buffer, sizeof(RpcData));
                    rpc.peer_id = sender;
                    rpc.received = true;
                    rpc_fifo.post(rpc);
                }
            } else {
                // data for DataSink
                uint8_t data_sink_id = decode_buffer[0] - 64;
                if (data_sink_id < BLUETOOTH_NUMBER_OF_DATA_SINKS) {
                    Mutex::Lock lock(data_sink_mutex);
                    if (data_sinks[data_sink_id].buffer && (size - 1) == data_sinks[data_sink_id].buffer_size) {
                        std::memcpy(data_sinks[data_sink_id].buffer, decode_buffer + 1, size - 1);
                        data_sinks[data_sink_id].lastSourceId = sender;
                        data_sinks[data_sink_id].newDataAvailable = true;
                    }
                    lock.unlock();
                }
            }
        } else {
            in_buffer[sender].data[in_buffer[sender].index] = buffer[i];
            ++in_buffer[sender].index;
            if (in_buffer[sender].index == sizeof(InBuffer::data)) {
                in_buffer[sender].waitForStartByte = true;
            }
        }
    }
}



bool registerRpcFunction(uint8_t rpc_id, RpcFunction callback) {
    if (rpc_id < BLUETOOTH_NUMBER_OF_RPCS) {
        Mutex::Lock lock(rpc_mutex);
        rpc_functions[rpc_id] = callback;
        lock.unlock();
        return true;
    } else {
        return false;
    }
}


bool callRpc(uint8_t destination, uint8_t rpc_id, uint64_t param) {
    if (destination < BLUETOOTH_NUMBER_OF_PEERS) {
        Rpc_t rpc;
        rpc.data.rpc_id = rpc_id;
        rpc.data.param = param;
        rpc.received = false;
        rpc.peer_id = destination;
        rpc_fifo.post(rpc);
        return true;
    } else {
        return false;
    }
}


static bool addDataSink(uint8_t data_sink_id, uint8_t* storage_buffer, size_t length) {
    if (data_sink_id >= BLUETOOTH_NUMBER_OF_DATA_SINKS || !storage_buffer) return false;

    Mutex::Lock lock(data_sink_mutex);
    data_sinks[data_sink_id] = DataSink(storage_buffer, length);
    lock.unlock();
    return true;
}
template<typename T> bool addDataSink(uint8_t data_sink_id, T* storage_buffer) {
    return addDataSink(data_sink_id, reinterpret_cast<uint8_t*>(storage_buffer), sizeof(*storage_buffer));
}

static bool getData(uint8_t data_sink_id, uint8_t* buffer, size_t length) {
    if (data_sink_id >= BLUETOOTH_NUMBER_OF_DATA_SINKS || !buffer) return false;

    Mutex::Lock lock(data_sink_mutex);
    if (length != data_sinks[data_sink_id].buffer_size) return false;
    std::memcpy(buffer, data_sinks[data_sink_id].buffer, length);
    data_sinks[data_sink_id].newDataAvailable = false;
    lock.unlock();
    return true;
}
template<typename T> bool getData(uint8_t data_sink_id, T* buffer) {
    return getData(data_sink_id, reinterpret_cast<uint8_t*>(buffer), sizeof(*buffer));
}


static bool addDataProvider(uint8_t data_provider_id, uint8_t* storage_buffer, size_t length, uint8_t destination) {
    if (data_provider_id >= BLUETOOTH_NUMBER_OF_DATA_PROVIDERS || !storage_buffer) return false;

    Mutex::Lock lock(data_provider_mutex);
    data_providers[data_provider_id] = DataProvider(storage_buffer, length, destination);
    lock.unlock();
    return true;
}
template<typename T> bool addDataProvider(uint8_t data_provider_id, T* storage_buffer, uint8_t destination) {
    return addDataProvider(data_provider_id, reinterpret_cast<uint8_t*>(storage_buffer), sizeof(*storage_buffer), destination);
}

static bool pushData(uint8_t data_provider_id, uint8_t* data, size_t length) {
    if (data_provider_id >= BLUETOOTH_NUMBER_OF_DATA_PROVIDERS || !data) return false;

    Mutex::Lock lock(data_provider_mutex);
    if (length != data_providers[data_provider_id].buffer_size) return false;
    std::memcpy(data_providers[data_provider_id].buffer, data, length);
    data_providers[data_provider_id].sendRequest = true;
    lock.unlock();
    return true;
}
template<typename T> bool pushData(uint8_t data_provider_id, T* data) {
    return pushData(data_provider_id, reinterpret_cast<uint8_t*>(data), sizeof(*data));
}


} // namespace Bluetooth
} // namespace TURAG
