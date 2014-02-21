#ifndef SYSTEMCONTROL_COMMON_BLUETOOTH_H
#define SYSTEMCONTROL_COMMON_BLUETOOTH_H

#include <tina++/tina.h>
#include <tina/config.h>

// -------------------------------------------------------------------
// High-Level Functions
// -------------------------------------------------------------------



// TODO:
// right now we use the data sink and data provider IDs directly as an index
// for the array the structures are stored in.
// Thus with the current implementation it's impossible to push data
// to data sinks of different peers when they have the same ID.
// Still, one advantage of the current implementation is its simplicity
// performance. Anyway it would be nice
// to have this cleaned up.


namespace TURAG {
namespace Bluetooth {
	
typedef void (*RpcFunction)(uint8_t sender, uint64_t param);

void init(void);
#ifndef TURAG_THREADS_RUN_FOREVER
 void quit(void);
#endif

void highlevelParseIncomingData(uint8_t sender, uint8_t* buffer, size_t buffer_size);

bool registerRpcFunction(uint8_t rpc_id, RpcFunction callback);
bool callRpc(uint8_t destination, uint8_t rpc_id, uint64_t param);

bool addDataSink(uint8_t data_sink_id, uint8_t* storage_buffer, size_t length);
bool getData(uint8_t data_sink_id, uint8_t* buffer, size_t length);
bool addDataProvider(uint8_t data_provider_id, uint8_t* storage_buffer, size_t length, uint8_t destination);
bool pushData(uint8_t data_provider_id, uint8_t* data, size_t length);

/**
 * Configures a data sink with the given ID. You need to supply a pointer to some struct
 * that is used to receive the data in background. You should never access this memory directly.
 * @param[in] data_sink_id
 * @param[in] storage_buffer
 * @return false if index was invalid, true otherwise
 */
template<typename T> _always_inline bool addDataSink(uint8_t data_sink_id, T* storage_buffer) {
    return addDataSink(data_sink_id, reinterpret_cast<uint8_t*>(storage_buffer), sizeof(*storage_buffer));
}

/**
 * Fetches data from a data sink.
 * @param[in] data_sink_id
 * @param[out] buffer
 * @return false if index was invalid, true otherwise
 */
template<typename T> _always_inline bool getData(uint8_t data_sink_id, T* buffer) {
    return getData(data_sink_id, reinterpret_cast<uint8_t*>(buffer), sizeof(*buffer));
}


/**
 * Configures a data provider with the given ID. You need to supply a pointer to some struct
 * that is used to store the data until it is sent. You should never access this memory directly.
 * @param[in] data_provider_id
 * @param[in] storage_buffer
 * @param[in] destination
 * @return false if index was invalid, true otherwise
 */
template<typename T> _always_inline bool addDataProvider(uint8_t data_provider_id, T* storage_buffer, uint8_t destination) {
    return addDataProvider(data_provider_id, reinterpret_cast<uint8_t*>(storage_buffer), sizeof(*storage_buffer), destination);
}

/**
 * Pushes new data into the data provider storage and enqueues the data for sending.
 * @param[in] data_provider_id
 * @param[in] data
 * @return false if index was invalid, true otherwise
 */
template<typename T> _always_inline bool pushData(uint8_t data_provider_id, T* data) {
    return pushData(data_provider_id, reinterpret_cast<uint8_t*>(data), sizeof(*data));
}


// -------------------------------------------------------------------
// Low-Level Functions & more
// implemented in device-specific source file
// called by high-level functions
// -------------------------------------------------------------------
		
void lowlevelInit(void);
bool write(uint8_t peer_id, uint8_t* buffer, size_t buffer_size);

bool isPeerConnected(uint8_t peer_id);
void setPeerStatus(uint8_t peer_id, bool active);
	
	
} // namespace Bluetooth
} // namespace TURAG




#endif // SYSTEMCONTROL_COMMON_BLUETOOTH_H
