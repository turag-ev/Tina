#define TURAG_DEBUG_LOG_SOURCE "Y"
//#define TURAG_DEBUG_LEVEL 5

#include "bluetooth_base.h"

#include <tina++/debug.h>
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

// we need this dirty function because we can't start a thread
// with a member function pointer.
void BluetoothBase::worker_thread_entry(void) {
    if (instance_) {
        instance_->worker_thread_func();
    }
}

void BluetoothBase::init(void) {
    for (int i = 0; i < BLUETOOTH_NUMBER_OF_PEERS; ++i) {
        peersEnabled[i].store(false, std::memory_order_relaxed);
        peerConnectionSuccessfulOnce[i].store(false, std::memory_order_relaxed);
    }

    lowlevelInit();

    instance_ = this;
    bluetooth_main_thread.start(BLUETOOTH_THREAD_PRIORITY, thread_entry);
    bluetooth_worker_thread.start(BLUETOOTH_WORKER_THREAD_PRIORITY, worker_thread_entry);
}

void BluetoothBase::main_thread_func(void) {
	CurrentThread::setName("bluetooth-high");

    turag_info("Bluetooth-High-Level thread started");

    QueueElement_t mail;
    Status connectionStatus[BLUETOOTH_NUMBER_OF_PEERS] {Status::disconnected};

    while (!bluetooth_main_thread.shouldTerminate()) {
        // this mechanism checks whether we already had a successful connection after enabling the
        // peer. This is mainly used to control the output of debug messages.
        for (int i = 0; i < BLUETOOTH_NUMBER_OF_PEERS; ++i) {
            Status newConnectionStatus = getConnectionStatusLowlevel(i);

            if (connectionStatus[i] != Status::connected && newConnectionStatus == Status::connected) {
                turag_infof("Peer %d: connection established", i);
                if (!peerConnectionSuccessfulOnce[i].load(std::memory_order_relaxed)) {
                    peerConnectionSuccessfulOnce[i].store(true, std::memory_order_relaxed);
                }
            } else if (connectionStatus[i] == Status::connected && newConnectionStatus != Status::connected) {
                turag_warningf("Peer %d: connection LOST", i);
            }
            connectionStatus[i] = newConnectionStatus;
        }

        if (!outQueue.wait(&mail, SystemTime::fromMsec(50))) {
            turag_debug("outQueue empty - continueing");
            continue;
        }
        if (!peersEnabled[mail.peer_id].load(std::memory_order_relaxed)) {
            // Remove element from queue if peer is enabled. This should only
            // happen on the rare occasion that the peer was disabled while some
            // old rpc calls were still waiting in the queue.
            outQueue.fetch();
            continue;
        }
        if (connectionStatus[mail.peer_id] != Status::connected) {

            // TODO:
            // remove dumb ThreadDelay. Replace outQueue with
            // some kind of PriorityQueue.
            // Current implementation will NOT work with more than one peer.

            Thread_delay(SystemTime::fromMsec(50));
            turag_debug("peer not connected, waiting 50 ms, sending nothing");

            continue;
        }

        if (mail.isRpc) {
            RpcData rpc {mail.index, mail.rpcParam };

            int buffer_size = Base64::encodeLength(rpc) + 2;
            uint8_t buffer[buffer_size];
            // start byte
            buffer[0] = 2;
            // data
            Base64::encode(rpc, buffer + 1);
            // end byte
            buffer[buffer_size - 1] = 3;

            if (write(mail.peer_id, buffer, buffer_size)) {
                // Remove element from queue only if peer is connected and writing was successful.
                // If it isn't connected then we are obviously waiting for a connection. If not
                // we should realize that when we try again next time.
                outQueue.fetch();
                turag_debugf("call Remote-RPC %d on peer %d", rpc.rpc_id, mail.peer_id);
            } else {
                turag_warningf("call Remote-RPC %d on peer %d - FAILED", rpc.rpc_id, mail.peer_id);
            }
        } else {
            Mutex::Lock lock(data_provider_mutex);

            ArrayBuffer<DataProvider, BLUETOOTH_NUMBER_OF_DATA_PROVIDERS>::iterator
                    provider = std::find_if(
                        data_providers.begin(), data_providers.end(),
                        [&] (const DataProvider& provider) {
                return provider.destination == mail.peer_id && provider.id == mail.index;
            });

            int encoded_buffer_size = Base64::encodeLength(provider->buffer_size) + 2;
            uint8_t buffer[encoded_buffer_size];
            // start byte
            buffer[0] = 2;
            // ID
            provider->buffer[0] = provider->id + 64;
            // data
            Base64::encode(provider->buffer, provider->buffer_size, buffer + 1);
            // end byte
            buffer[encoded_buffer_size - 1] = 3;
            lock.unlock();

            // write to bluetooth, we unlock the mutex in case the writing takes a bit more time
            if (write(mail.peer_id, buffer, encoded_buffer_size)) {
                turag_debugf("DataProvider %d push to %d", mail.index, mail.peer_id);
                outQueue.fetch();
            } else {
                turag_warningf("DataProvider %d push to %d - FAILED", mail.index, mail.peer_id);
            }

        }
    }
}

void BluetoothBase::worker_thread_func(void) {
	CurrentThread::setName("bluetooth-worker");

    turag_info("Bluetooth-Worker thread started");

    while (!bluetooth_worker_thread.shouldTerminate()) {
        QueueElement_t mail;

        if (!inQueue.fetch(&mail, SystemTime::fromMsec(200))) continue;
        if (!peersEnabled[mail.peer_id].load(std::memory_order_relaxed)) continue;

        if (mail.isRpc) {
            Mutex::Lock lock(rpc_mutex);
            // we don't need to check the ID here. Calls with invalid IDs
            // are rejected upon reception and not queued.
            RpcFunction callback = rpc_functions[mail.index];
            lock.unlock();

            if (callback) {
                turag_debugf("call RPC %d", mail.index);
                (*callback)(mail.peer_id, mail.rpcParam);
            }
        } else {
            Mutex::Lock lock(data_sink_mutex);

            // we don't need to check the ID here. Calls with invalid IDs
            // are rejected upon reception and not queued.
            DataSinkNotificationHandler handler = data_sinks[mail.index].notificationHandler;
            lock.unlock();

            if (handler) {
                turag_debugf("call data sink notifier %d", mail.index);
                (*handler)(mail.index);
            }
        }
    }

}

void BluetoothBase::highlevelParseIncomingData(uint8_t sender, uint8_t* buffer, size_t buffer_size) {
    if (sender >= BLUETOOTH_NUMBER_OF_PEERS) return;

    // ignore traffic from deactivated peers
    if (!peersEnabled[sender].load(std::memory_order_relaxed)) {
        in_buffer[sender].waitForStartByte = true;
        return;
    }

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
                    turag_criticalf("RPC %d recv from %d--> ID invalid", decode_buffer[0], sender);
                } else if (decoded_size != sizeof(RpcData)) {
                    turag_criticalf("RPC %d recv from %d --> package size mismatch (%d)", decode_buffer[0], sender, static_cast<int>(decoded_size));
                } else {
                    RpcData* data = reinterpret_cast<RpcData*>(decode_buffer);

                    QueueElement_t element(data->rpc_id, sender, data->param);

                    if (!inQueue.post(element, SystemTime::fromMsec(10))) {
                        turag_criticalf("Received rpc %d from peer %d but couldn't enqueue it for execution; inQueue full", data->rpc_id, sender);
                    } else {
                        turag_debugf("RPC %d recv from %d --> queued for exec", decode_buffer[0], sender);
                    }
                }
            } else {
                // data for DataSink
                uint8_t data_sink_id = decode_buffer[0] - 64;
                if (data_sink_id < BLUETOOTH_NUMBER_OF_DATA_SINKS) {
                    Mutex::Lock lock(data_sink_mutex);
                    if (!data_sinks[data_sink_id].buffer) {
                        turag_warningf("received data for data sink %d but buffer is nullptr", data_sink_id);
                    } else if (decoded_size != data_sinks[data_sink_id].buffer_size) {
                        turag_warningf("received data for data sink %d but buffer size doesn't match", data_sink_id);
                    } else if (decoded_size <= 1) {
                        turag_warningf("received data for data sink %d without user data", data_sink_id);
                    } else {
                        turag_debugf("DataSink %d recv data from %d", data_sink_id, sender);
                        std::memcpy(data_sinks[data_sink_id].buffer, &decode_buffer, decoded_size);

                        if (data_sinks[data_sink_id].notificationHandler) {
                            if (!inQueue.postUnique(QueueElement_t(data_sink_id, sender), SystemTime::fromMsec(10))) {
                                turag_criticalf("couldn't enqueue execution of notification handler for data sink %d; inQueue full", data_sink_id);
                            } else {
                                turag_debugf("enqueued notification handler of data sink %d for execution", data_sink_id);
                            }
                        }
                    }
                } else {
                    turag_criticalf("DataSink %d recv data from %d -> ID invalid!", data_sink_id, sender);
                }
            }
        } else {
            in_buffer[sender].data[in_buffer[sender].index] = buffer[i];
            ++in_buffer[sender].index;
            if (in_buffer[sender].index == sizeof(InBuffer::data)) {
                in_buffer[sender].waitForStartByte = true;
                turag_warningf("Overflow in recv-buffer for peer %d", sender);
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
        turag_error("invalid RPC ID");
        return false;
    }
}


bool BluetoothBase::callRpc(uint8_t destination, uint8_t rpc_id, uint64_t param) {
    if (destination >= BLUETOOTH_NUMBER_OF_PEERS) {
        turag_error("invalid peer ID");
        return false;
    } else if (!peersEnabled[destination].load(std::memory_order_relaxed)) {
		turag_warningf("peer not enabled");
		return false;
    } else {
        turag_debug("trying to post rpc to outQueue...");
        if (!outQueue.post(QueueElement_t(rpc_id, destination, param), SystemTime::fromMsec(10), BLUETOOTH_OUTQUEUE_LEAVE_SPACE_WHEN_QUEUING_RPC)) {
            turag_criticalf("Couldn't call RPC %d on peer %d; outQueue full", rpc_id, destination);
            return false;
        } else {
            return true;
        }
    }
}

bool BluetoothBase::addDataSink(uint8_t data_sink_id, uint8_t* storage_buffer, size_t length, DataSinkNotificationHandler handler) {
    if (data_sink_id >= BLUETOOTH_NUMBER_OF_DATA_SINKS || !storage_buffer) {
        turag_error("invalid DataSink ID or storage pointer zero");
        return false;
    } else if (length > 92 + 1) {
        turag_error("DataSinks can not hold data bigger than 92 byte");
		return false;
    } else {
        Mutex::Lock lock(data_sink_mutex);
        if (data_sinks[data_sink_id].buffer) {
            turag_warningf("datasink %d was overwritten", data_sink_id);
        }
        turag_infof("added datasink %d with size %d", (int)data_sink_id, (int)length);
        data_sinks[data_sink_id] = DataSink(storage_buffer, length, handler);
        return true;
    }
}

bool BluetoothBase::getData(uint8_t data_sink_id, uint8_t* buffer, size_t length) {
    if (data_sink_id >= BLUETOOTH_NUMBER_OF_DATA_SINKS || !buffer) {
        turag_error("invalid DataSink ID or buffer pointer zero");
        return false;
    } else {
        Mutex::Lock lock(data_sink_mutex);
        if (!data_sinks[data_sink_id].buffer) {
            turag_errorf("Tried to read data from unitialized data sink %d", (int)data_sink_id);
        } else if (length != data_sinks[data_sink_id].buffer_size) {
            turag_errorf("incorrect data length for DataSink %d (given: %d, expected: %d)", (int)data_sink_id, (int)length, (int)data_sinks[data_sink_id].buffer_size);
            return false;
        }
        std::memcpy(buffer, data_sinks[data_sink_id].buffer, length);
        return true;
    }
}


bool BluetoothBase::addDataProvider(uint8_t destination, uint8_t data_provider_id, uint8_t* storage_buffer, size_t length) {
    if (!storage_buffer) {
        turag_error("storage pointer zero");
        return false;
    } else if (length > 92 + 1) {
        turag_error("DataProvider can not hold data bigger than 92 byte");
        return false;
    } else if (destination > BLUETOOTH_NUMBER_OF_PEERS) {
        turag_error("DataSinks invalid peer ID");
        return false;
    } else {
        Mutex::Lock lock(data_provider_mutex);
        ArrayBuffer<DataProvider, BLUETOOTH_NUMBER_OF_DATA_PROVIDERS>::iterator provider = std::find_if(data_providers.begin(), data_providers.end(), [&] (const DataProvider& provider) { return provider.destination == destination && provider.id == data_provider_id; });
        if (provider != data_providers.end()) {
            turag_warningf("dataprovider %d was overwritten", data_provider_id);
            provider->buffer = storage_buffer;
            provider->buffer_size = length;
            provider->destination = destination;
            provider->id = data_provider_id;
        } else {
            turag_infof("added dataprovider id: %d peer: %d size: %d", (int)data_provider_id, (int)destination, (int)length);
            data_providers.emplace_back(DataProvider(storage_buffer, length, destination, data_provider_id));
        }
        return true;
    }
}

bool BluetoothBase::pushData(uint8_t destination, uint8_t data_provider_id, const uint8_t* data, size_t length) {
    Mutex::Lock lock(data_provider_mutex);

    ArrayBuffer<DataProvider, BLUETOOTH_NUMBER_OF_DATA_PROVIDERS>::iterator
            provider = std::find_if(
                data_providers.begin(), data_providers.end(),
                [&] (const DataProvider& provider) {
        return provider.destination == destination && provider.id == data_provider_id;
    });

    if (provider == data_providers.end()) {
        turag_error("specified data provider couldn't be found");
        return false;
    } else if (!peersEnabled[destination].load(std::memory_order_relaxed)) {
        return false;
    } else {
        if (length != provider->buffer_size) {
            turag_errorf("incorrect data length (pushData dest=%d id=%d)", destination, data_provider_id);
            return false;
        }
        std::memcpy(provider->buffer, data, length);

        if (!outQueue.postUnique(QueueElement_t(data_provider_id, destination), SystemTime::fromMsec(10))) {
            turag_criticalf("couldn't push data to provider %d, outQueue full", data_provider_id);
            return false;
        } else {
            return true;
        }
    }
}


bool BluetoothBase::pushData(uint8_t destination, uint8_t data_provider_id) {
    Mutex::Lock lock(data_provider_mutex);

    ArrayBuffer<DataProvider, BLUETOOTH_NUMBER_OF_DATA_PROVIDERS>::iterator
            provider = std::find_if(
                data_providers.begin(), data_providers.end(),
                [&] (const DataProvider& provider) {
        return provider.destination == destination && provider.id == data_provider_id;
    });

    if (provider == data_providers.end()) {
        turag_error("specified data provider couldn't be found");
        return false;
    } else if (!peersEnabled[destination].load(std::memory_order_relaxed)) {
        return false;
    } else {
        if (!outQueue.postUnique(QueueElement_t(data_provider_id, destination), SystemTime::fromMsec(10))) {
            turag_criticalf("couldn't push data to provider %d, outQueue full", data_provider_id);
            return false;
        } else {
            return true;
        }
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

    if (enabled) {
        if (getConnectionStatus(peer_id) != Status::connected) {
            turag_infof("Peer %d enabled -> waiting for successful connection", peer_id);
        }
    } else {
        turag_infof("Peer %d disabled", peer_id);
    }

}



} // namespace TURAG
