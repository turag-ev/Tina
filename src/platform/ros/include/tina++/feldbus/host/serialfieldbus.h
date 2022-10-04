#ifndef PLATFORM_ROS_INCLUDE_TINAPP_FELDBUS_HOST_SERIALFIELDBUS_H
#define PLATFORM_ROS_INCLUDE_TINAPP_FELDBUS_HOST_SERIALFIELDBUS_H
#include <tina++/tina.h>
#include <tina++/feldbus/host/feldbusabstraction.h>
#include <boost/asio.hpp>
#include <chrono>
#include <string>

namespace TURAG {
namespace Feldbus {

class SerialFieldbus: public Feldbus::FeldbusAbstraction
{
public:
    SerialFieldbus(const std::string& device, unsigned baudrate);
    void clearBuffer() override;
protected:
    bool doTransceive(const uint8_t* transmit, int* transmit_length, uint8_t* receive, int* receive_length, bool delayTransmission) override;
private:
    void writeHandler(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void readHandler(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void timeoutHandler(const boost::system::error_code& ec);

    boost::asio::io_service io_service_;
    boost::asio::serial_port port_;
    boost::asio::deadline_timer timeout_;
    boost::system::error_code write_error_;
    boost::system::error_code read_error_;

    SystemTime transmission_delay_;

    size_t bytes_written_, bytes_read_;
    uint8_t* receive_;
    size_t transmit_length_, receive_length_;
    std::chrono::time_point<std::chrono::high_resolution_clock> send_time_;
};

} // namespace Feldbus
} // namespace TURAG

#endif // PLATFORM_ROS_INCLUDE_TINAPP_FELDBUS_HOST_SERIALFIELDBUS_H
