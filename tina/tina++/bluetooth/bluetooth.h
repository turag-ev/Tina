#ifndef SYSTEMCONTROL_COMMON_BLUETOOTH_H
#define SYSTEMCONTROL_COMMON_BLUETOOTH_H

#include <tina++/tina.h>

// -------------------------------------------------------------------
// High-Level Functions
// -------------------------------------------------------------------

namespace TURAG {
namespace Bluetooth {
	
typedef void (*RpcFunction)(uint8_t sender, uint64_t param);

void init(void);
void highlevelParseIncomingData(uint8_t sender, uint8_t* buffer, size_t buffer_size);

bool registerRpcFunction(uint8_t rpc_id, RpcFunction callback);
bool callRpc(uint8_t destination, uint8_t rpc_id, uint64_t param);

/**
 * Configures a data sink with the given ID. You need to supply a pointer to some struct
 * that is used to receive the data in background. You should never access this memory directly.
 * @param[in] data_sink_id
 * @param[in] storage_buffer
 * @return false if index was invalid, true otherwise
 */
template<typename T> bool addDataSink(uint8_t data_sink_id, T* storage_buffer);
/**
 * Fetches data from a data sink.
 * @param[in] data_sink_id
 * @param[out] buffer
 * @return false if index was invalid, true otherwise
 */
template<typename T> bool getData(uint8_t data_sink_id, T* buffer);


/**
 * Configures a data provider with the given ID. You need to supply a pointer to some struct
 * that is used to store the data until it is sent. You should never access this memory directly.
 * @param[in] data_provider_id
 * @param[in] storage_buffer
 * @param[in] destination
 * @return false if index was invalid, true otherwise
 */
template<typename T> bool addDataProvider(uint8_t data_provider_id, T* storage_buffer, uint8_t destination);
/**
 * Pushes new data into the data provider storage and enqueues the data for sending.
 * @param[in] data_provider_id
 * @param[in] data
 * @return false if index was invalid, true otherwise
 */
template<typename T> bool pushData(uint8_t data_provider_id, T* data);


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
