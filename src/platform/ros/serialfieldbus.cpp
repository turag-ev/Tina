#include <tina++/feldbus/host/serialfieldbus.h>
#include <ros/console.h>

using namespace boost::asio;
using namespace std::chrono;

namespace TURAG {
namespace Feldbus {

SerialFieldbus::SerialFieldbus(const std::string& device, unsigned baudrate):
    Feldbus::FeldbusAbstraction("SerialFieldbus"),
    io_service_(),
    port_(io_service_),
    timeout_(io_service_)
{
    transmission_delay_ = SystemTime::fromSecDouble(15.0 / baudrate);

    try {
        port_.open(device);
    } catch(std::exception& ex) {
        ROS_ERROR("SerialFieldbus: Exception while trying to open serial port %s: %s", device.c_str(), ex.what());
        return;
    }

    port_.set_option(serial_port::baud_rate(baudrate));
    port_.set_option(serial_port::flow_control(serial_port::flow_control::none));
    port_.set_option(serial_port::character_size(8));
    port_.set_option(serial_port::parity(serial_port::parity::none));
    port_.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
    if(!port_.is_open()) {
        ROS_ERROR("SerialFieldbus: Couldn't open serial port %s.", device.c_str());
        return;
    } else {
        ROS_INFO("SerialFieldbus: Successfully opened serial port %s with %d baud.", device.c_str(), baudrate);
    }
}

void SerialFieldbus::clearBuffer() {
    ::tcflush(port_.lowest_layer().native_handle(), TCIOFLUSH);
}

void SerialFieldbus::writeHandler(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    ROS_DEBUG("SerialFieldbus: wrote %lu bytes", bytes_transferred);
    write_error_ = ec;
    bytes_written_ = bytes_transferred;
    if(write_error_)
        return;

    if(receive_ && receive_length_) {
        ROS_DEBUG("SerialFieldbus: reading %lu bytes", receive_length_);
        async_read(port_, boost::asio::buffer(receive_, receive_length_), boost::bind(&SerialFieldbus::readHandler, this, _1, _2));
    } else {
        timeout_.cancel();
    }
}

void SerialFieldbus::readHandler(const boost::system::error_code &ec, std::size_t bytes_transferred) {
    ROS_DEBUG("SerialFieldbus: read %lu bytes", bytes_transferred);
    ROS_DEBUG_STREAM("SerialFieldbus: Duration: " << duration_cast<microseconds>(high_resolution_clock::now() - send_time_).count() << "us");
    read_error_ = ec;
    bytes_read_ = bytes_transferred;
    timeout_.cancel();
}

void SerialFieldbus::timeoutHandler(const boost::system::error_code& ec) {
    if(ec == boost::asio::error::operation_aborted)
        return;
    ROS_ERROR_THROTTLE(5.0, "SerialFieldbus: Fieldbus transmission timeout!");
    port_.cancel();
}

bool SerialFieldbus::doTransceive(const uint8_t* transmit, int* transmit_length,
                                  uint8_t* receive, int* receive_length, bool delay) {
    if(delay)
        CurrentThread::delay(transmission_delay_);

    if(!port_.is_open()) {
        ROS_ERROR_THROTTLE(5.0, "SerialFieldbus: Transcieve failed: Serial port not open.");
        return false;
    }
    bytes_written_ = bytes_read_ = 0;
    read_error_.clear();
    write_error_.clear();

    receive_ = receive;

    if(transmit_length)
        transmit_length_ = *transmit_length;
    else
        transmit_length_ = 0;

    if(receive_length)
        receive_length_ = *receive_length;
    else
        receive_length_ = 0;

    timeout_.expires_from_now(boost::posix_time::milliseconds(50));
    timeout_.async_wait(boost::bind(&SerialFieldbus::timeoutHandler, this, _1));

    io_service_.reset();
    send_time_ = high_resolution_clock::now();
    if(transmit_length_)
        async_write(port_, boost::asio::buffer(transmit,transmit_length_), boost::bind(&SerialFieldbus::writeHandler, this, _1, _2));
    else if(receive_length_)
        async_read(port_, boost::asio::buffer(receive,receive_length_), boost::bind(&SerialFieldbus::readHandler, this, _1, _2));

    io_service_.run();

    if(transmit_length)
        *transmit_length = bytes_written_;
    if(receive_length)
        *receive_length = bytes_read_;

    if(write_error_) {
        ROS_ERROR_THROTTLE(5.0, "SerialFieldbus: Fieldbus write error: %s", write_error_.message().c_str());
        return false;
    }
    if(read_error_ ) {
        ROS_ERROR_THROTTLE(5.0, "SerialFieldbus: Fieldbus read error: %s", read_error_.message().c_str());
        return false;
    }
    return true;
}

} // namespace Feldbus
} // namespace TURAG
