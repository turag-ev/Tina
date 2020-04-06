#ifndef ROS_H
#define ROS_H

#include <type_traits>

#include <ch.h>
#include <hal.h>

#include <ros/node_handle.h>

namespace ros {

class ChibiOSHardware {
    public:
        void init() { }

        template<typename StreamType>
        void setChannel(StreamType* channel) {
            // TODO: Verify correct StreamType by comparing VMT
            channel_ = reinterpret_cast<BaseChannel*>(channel);
        }

        int read() {
            return chnGetTimeout(channel_, TIME_IMMEDIATE);
        }

        void write(uint8_t* data, int length) {
            chnWrite(channel_, data, length);
        }

        unsigned long time() {
            return TIME_I2MS(chVTGetSystemTimeX());
        }

    protected:
        BaseChannel* channel_ = nullptr;
};

using NodeHandle = NodeHandle_<ChibiOSHardware, 50, 50, 1024, 1024>;

} // namespace ros

#endif /* ROS_H */
