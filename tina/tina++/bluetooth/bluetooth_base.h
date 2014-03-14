/*!
 * \file tina++/bluetooth/bluetooth_base.h
 * \author Martin Oemus <martin@oemus.net>
 * \date March 2013
 *
 * \brief Contains the bluetooth module.
 */


#ifndef TINAPP_BLUETOOTH_BLUETOOTHBASE_H
#define TINAPP_BLUETOOTH_BLUETOOTHBASE_H

#include <tina++/tina.h>
#include <tina++/thread.h>
#include <tina++/container/thread_fifo.h>
#include <tina++/container/array_buffer.h>
#include <atomic>
#include <array>

#include <tina/bluetooth_config.h>


/*!
 * \defgroup bluetooth Bluetooth
 * \section bluetoothmodule Bluetooth module
 * This module offers various ways to exchange data via bluetooth between peers in a
 * thread-safe and flexible manner. It is implemented as an abstract base class
 * which needs to be extended with hardware specifics.
 *
 * All the time-consuming communication work is done
 * within a thread while all public access functions return within a minimum of time.
 * It builds on top of a set of low-level
 * functions that need to be implemented in the derived subclass. This abstraction layer is responsible
 * for mapping the bluetooth hardware addresses to peer IDs which start with 0, for piping
 * received input into the appropriate high-level functions and for writing data back to
 * the hardware. The mapping of connected peers is limited to 256.
 *
 * This module requires the header <tina/bluetooth_config.h> to be availaible which needs to define
 * the following Symbols:
 * - BLUETOOTH_NUMBER_OF_PEERS
 * - BLUETOOTH_NUMBER_OF_RPCS
 * - BLUETOOTH_NUMBER_OF_DATA_SINKS
 * - BLUETOOTH_NUMBER_OF_DATA_PROVIDERS
 * - BLUETOOTH_THREAD_STACK_SIZE
 * - BLUETOOTH_THREAD_PRIORITY
 * - BLUETOOTH_THREAD_RPC_WAIT_MS
 *
 * \subsection Implementingsubclasses Implementing subclasses
 * When implementing the subclass you should make sure it can't be instantiated more than once
 * as this wouldn't make any sense. For this purpose you should override the get()-function. Let's
 * look at a small example. First we need some includes:
 * \snippet bluetooth.h Includes
 * Then we declare our subclass:
 * \snippet bluetooth.h class
 * There are a few interesting things to consider with this code:
 * - the constructor is protected. This inhibits the creation of any instances from outside the class.
 * - we have a static variable instance_ that holds our one and only instance of our class
 * - we have the static function get() which returns a pointer to this instance
 *
 * You should be aware that implementing a singleton with a globally allocated static like in this case
 * is problematic if you have other singletons taht are implemented in the same way depend on it.
 * But at least it's thread-safe.
 *
 *
 * \subsection rpc RPCs
 * Every peer can offer RPCs. You need to register them at the bluetooth module with
 * an ID that starts with zero. After registering the RPC any connected peer can call
 * the RPC that knows the correct RPC id which will then result in the callback function getting called. The calling
 * peer can pass along an argument of 8 bytes size. A maximum number of 64 RPCs are supported.
 *
 * If you call an RPC, it is enqueued for transmit. The bluetooth thread handles transmitting
 * the data and takes care, that the call actually happens. Thus calling a RPC is considered
 * error-free and the function invoking the RPC returns quickly. At the same time RPCs can not
 * return any results.
 *
 * \subsection data Arbitrary data
 * If you want to transceive larger amounts of data you can define a so-called data sink.
 * It consists of a storage buffer that is associated with a data sink id. A number of up to
 * 64 data sinks is supported. Any other peer
 * is allowed to transmit data to into this data sink. To do that, this peer has to configure
 * a data provider with the target's id and data sink id.
 * If you want to push data from a data provider to a data sink, the data is copied into the data
 * provider's storage. Then the bluetooth thread will transmit the data. The receiving peer will
 * then copy the data into its storage buffer. Some other thread can then access this data.
 *
 */

namespace TURAG {

/*!
 * \addtogroup bluetooth
 * @{
 */

/*!
 * \brief Base class implementing the high level functionality of
 * the %Bluetooth module.
 */
class BluetoothBase {
public:
    /*!
     * @brief Type for RPC-functions.
     */
    typedef void (*RpcFunction)(uint8_t sender, uint64_t param);

    /*!
     * @brief Wrapper-structure for data sinks.
     */
    template <typename T>
    struct SinkData {
        uint8_t id;     ///< for internal use only.
        T data;         ///< data structure that will contain received data.
    } _packed;

    /*!
     * @brief Wrapper-structure for data providers.
     */
    template <typename T>
    struct PushData {
        uint8_t id;     ///< for internal use only.
        T data;         ///< data structure that contains data to send.
    } _packed;


    /*!
     * \brief Connection status.
     */
    enum class Status {
        disconnected,   ///< Peer is disconnected.
        connecting,     ///< Peer is trying to establish a connection.
        connected       ///< Peer is connected and ready to communicate.
    };

    /*!
     * \brief %Bluetooth thread.
     */
    // can't be private because simulation needs access
    Thread<BLUETOOTH_THREAD_STACK_SIZE> bluetooth_main_thread;

    inline Thread<BLUETOOTH_THREAD_STACK_SIZE> *getThreadPtr(void) {
        return &bluetooth_main_thread;
    }

    /*!
     * \brief Initializes the bluetooth-module.
     * \details Starts the bluetooth-thread and does all other required intializations.
     */
    void init(void);

    /*!
     * \brief Registers a function to be called via RPC.
     * \param[in] rpc_id ID of the RPC.
     * \param[in] callback Function handling the RPC-call.
     * \return True on success, false otherwise.
     * \details This function registers a function to be callable with the given RPC-ID. You need to call this function
     * if your application needs to respond to this particular RPC.
     *
     * This function fails if the RPC-ID is greater or equal to the number of supported RPCs.
     */
    bool registerRpcFunction(uint8_t rpc_id, RpcFunction callback);

    /*!
     * \brief Call RPC with given RPC-ID on given peer.
     * \param[in] destination Peer-ID to call the RPC on.
     * \param[in] rpc_id RPC-ID to call.
     * \param[in] param Parameter to pass to the RPC-function.
     * \return True on success, false otherwise.
     * \details This function fails if the destination peer-ID equals or exceeds the number of
     * remote peers or if the remote peer was disabled.
     */
    bool callRpc(uint8_t destination, uint8_t rpc_id, uint64_t param);


    /**
     * @brief Configures a data sink with the given ID.
     * @param[in] data_sink_id ID of data sink to configure.
     * @param[in] storage_buffer Pointer to data that is used to store the received data.
     * @return False if index was invalid, true otherwise.
     * @details  You need to supply a pointer to some struct
     * that is used to receive the data in background.
     * @note Never access the given struct directly. It is used exclusively by
     * the bluetooth-thread.
     */
    template<typename T> _always_inline bool addDataSink(uint8_t data_sink_id, SinkData<T>* storage_buffer) {
        return addDataSink(data_sink_id, reinterpret_cast<uint8_t*>(storage_buffer), sizeof(*storage_buffer));
    }

    /**
     * @brief Fetches data from a data sink.
     * @param[in] data_sink_id Data sink ID to fetch data from.
     * @param[out] buffer Pointer to memory where the data sink's data will be copied to.
     * @return False if index was invalid, true otherwise.
     */
    template<typename T> _always_inline bool getData(uint8_t data_sink_id, SinkData<T>* buffer) {
        return getData(data_sink_id, reinterpret_cast<uint8_t*>(buffer), sizeof(*buffer));
    }


    /**
     * @brief Configures a data provider with the given ID.
     * @param[in] destination ID of the peer the data will be pushed to.
     * @param[in] data_provider_id ID of the destination's data sink ID this provider should push data into.
     * @param[in] storage_buffer Pointer to memory that will be used to buffer the data before sending it.
     * @return False if destination or storage_buffer was invalid, true otherwise.
     * @details  You need to supply a pointer to some struct
     * that is used to store the data until it is sent.
     * @note Never access the given struct directly. It is used exclusively by
     * the bluetooth-thread.
     */
    template<typename T> _always_inline bool addDataProvider(uint8_t destination, uint8_t data_provider_id, PushData<T>* storage_buffer) {
        return addDataProvider(destination, data_provider_id, reinterpret_cast<uint8_t*>(storage_buffer), sizeof(*storage_buffer));
    }

    /**
     * @brief Pushes new data into the data provider storage and enqueues the data for sending.
     * @param[in] destination ID of the peer the data will be pushed to.
     * @param[in] data_provider_id ID of the destination's data sink ID the data should be pushed into.
     * @param[in] data Data to push.
     * @return True on success, false otherwise.
     * @details For this function to succeed, you need to configure a data provider with the exact same
     * combination of destination and data_provider_id.
     *
     * This function will fail if the remote peer is disabled.
     */
    template<typename T> _always_inline bool pushData(uint8_t destination, uint8_t data_provider_id, const PushData<T>& data) {
        return pushData(destination, data_provider_id, reinterpret_cast<const uint8_t*>(&data), sizeof(data));
    }

    /**
     * @brief Enqueues the data that is currently in the associated storage for sending.
     * @param[in] destination ID of the peer the data will be pushed to.
     * @param[in] data_provider_id ID of the destination's data sink ID the data should be pushed into.
     * @return True on success, false otherwise.
     * @details For this function to succeed, you need to configure a data provider with the exact same
     * combination of destination and data_provider_id.
     *
     * This function will fail if the remote peer is disabled.

     * This function is useful, if you have several peers whose data sinks are logically the same.
     * Then you can configure several data providers with the same storage buffer which needs to be updated
     * only once, for example wehn pushing to the first peer. The remaining peers can be updated with this
     * version of pushData.
     */
    template<typename T> _always_inline bool pushData(uint8_t destination, uint8_t data_provider_id) {
        return pushData(destination, data_provider_id);
    }

    /*!
     * \brief Returns connection status of specified peer.
     * \param[in] peer_id ID of peer to get connection status from.
     * \return Connection status.
     * \details If the peer was enabled, this function returns its physical connection
     * status. Otherwise it returns Status::disconnected.
     */
    Status getConnectionStatus(uint8_t peer_id);


    /*!
     * \brief Returns whether there was any successful connection since enabling the peer.
     * \param[in] peer_id ID of peer to get information status from.
     * \return True if there was ever a connection, false otherwise.
     * \details This function returning false can either mean that you called it to soon
     * after enabling the peer or that the peer is physically out of reach.
     *
     * You you should use this function when you want to check whether a peer is generally available
     * rather then getConnectionStatus() as the latter might return Status::disconnected due to
     * a temporary loss of connection.
     */
    bool getConnectionWasSuccessfulOnce(uint8_t peer_id) {
        return peerConnectionSuccessfulOnce[peer_id].load(std::memory_order_relaxed);
    }

    /*!
     * \brief Enables or disables a peer.
     * \param[in] peer_id Peer ID to enable/disable.
     * \param[in] enabled True: enable the peer, false: disable the peer.
     * \details If you disable a peer, every communication attempt with it
     * is blocked, all queued RPC calls are removed and push requests for data providers
     * are cancelled. Incoming data is ignored and already received data which was not
     * processed yet is trashed. It is not guaranteed
     * that the actual physical connection to the peer is cancelled.
     *
     * If you enable a peer, the low level layer is obligated to establish a
     * connection as quickly as possible.
     *
     * Disabling a peer does not affect the transmission of already queued RPC-calls
     * and pushed data.
     *
     *
     */
    void setPeerEnabled(uint8_t peer_id, bool enabled);

    /*!
     * \brief Returns an instance of the class.
     * \return nullptr.
     * \details This function should be overridden by a subclass to give access to
     * the single instance of the subclass.
     */
    static BluetoothBase* get(void) { return nullptr; }

protected:
    BluetoothBase(void) :
        rpc_functions{0} { }

    /*!
     * \brief Parses incoming raw data.
     * \param sender Peer ID of the sender of the data.
     * \param buffer Pointer to the data.
     * \param buffer_size Number of bytes contained in the buffer.
     * \details This function needs to be called by the the part of the hardware
     * abstraction layer that handles incoming data. It should pass through this data
     * completely unaltered.
     */
    void highlevelParseIncomingData(uint8_t sender, uint8_t* buffer, size_t buffer_size);

    /*!
     * \brief Low level initializations.
     * \details This function is called by init(). It is supposed to do all the steps
     * necessary to get the bluetooth hardware up and running.
     */
    // this function should be private - it's only protected to get it into the doxygen docs
    virtual void lowlevelInit(void) = 0;

    /*!
     * \brief Write data to a peer.
     * \param[in] peer_id Peer id to send data to.
     * \param[in] buffer Pointer to data.
     * \param[in] buffer_size Size of data.
     * \return True if transmission was succesful, false otherwise.
     * \details This function is only called in the context of the bluetooth thread
     * and therefore doesn't need to be thread-safe.
     *
     * The high level layer will never call this function, if the peer was disabled,
     * so it's never necessary to inhibit the transmission if writing is physically possible.
     *
     */
    // this function should be private - it's only protected to get it into the doxygen docs
    virtual bool write(uint8_t peer_id, uint8_t* buffer, size_t buffer_size) = 0;


    /*!
     * \brief Returns the physical connection status of specified peer.
     * \param[in] peer_id
     * \return Connection status.
     * \note This function needs to be thread-safe.
     */
    // this function should be private - it's only protected to get it into the doxygen docs
    virtual Status getConnectionStatusLowlevel(uint8_t peer_id) = 0;

    /*!
     * \brief Request or release the physical connection to a peer.
     * \param[in] peer_id Peer ID to enable/disable.
     * \param[in] enabled True: enable the peer, false: disable the peer.
     * \details This functions is called by the high-level implementation when a peer is enabled
     * or disabled. When called with true, the hardware layer is requested to establish a connection
     * to the peer. When called with false, it is allowed to release this connection yet not obligated.
     * \note This function needs to be thread-safe.
     */
    // this function should be private - it's only protected to get it into the doxygen docs
    virtual void setPeerEnabledLowlevel(uint8_t peer_id, bool enabled) = 0;

private:
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
        std::atomic_bool sendRequest;
        uint8_t destination;
        uint8_t id;

        DataProvider() :
            buffer(nullptr),
            buffer_size(0),
            destination(0), id(0) { }

        DataProvider(uint8_t* buf, size_t buf_size, uint8_t dest, uint8_t id_) :
            buffer(buf),
            buffer_size(buf_size),
            sendRequest(false),
            destination(dest), id(id_) { }

        DataProvider(const DataProvider& provider) :
            buffer(provider.buffer),
            buffer_size(provider.buffer_size),
            destination(provider.destination),
            id(provider.id) {
            sendRequest.store(provider.sendRequest.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }
    };

    struct DataSink {
        uint8_t* buffer;
        size_t buffer_size;

        DataSink() :
            buffer(nullptr),
            buffer_size(0) { }

        DataSink(uint8_t* buf, size_t buf_size) :
            buffer(buf),
            buffer_size(buf_size) { }
    };

    RpcFunction rpc_functions[BLUETOOTH_NUMBER_OF_RPCS];
    Mutex rpc_mutex;

    ArrayBuffer<DataProvider, BLUETOOTH_NUMBER_OF_DATA_PROVIDERS> data_providers;
    Mutex data_provider_mutex;

    DataSink data_sinks[BLUETOOTH_NUMBER_OF_DATA_SINKS];
    Mutex data_sink_mutex;

    std::array<std::atomic_bool, BLUETOOTH_NUMBER_OF_PEERS> peersEnabled;
    std::array<std::atomic_bool, BLUETOOTH_NUMBER_OF_PEERS> peerConnectionSuccessfulOnce;

    ThreadFifo<Rpc_t, 32> rpc_fifo;

    InBuffer in_buffer[BLUETOOTH_NUMBER_OF_PEERS];


    bool addDataSink(uint8_t data_sink_id, uint8_t* storage_buffer, size_t length);
    bool getData(uint8_t data_sink_id, uint8_t* buffer, size_t length);
    bool addDataProvider(uint8_t destination, uint8_t data_provider_id, uint8_t* storage_buffer, size_t length);
    bool pushData(uint8_t destination, uint8_t data_provider_id, const uint8_t* data, size_t length);
    bool pushData(uint8_t destination, uint8_t data_provider_id);

    void main_thread_func(void);

    static BluetoothBase* instance_;
    static void thread_entry(void);
};

///@}


} // namespace TURAG




#endif // TINAPP_BLUETOOTH_BLUETOOTHBASE_H
