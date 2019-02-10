#ifndef ROS_H
#define ROS_H

#include "ch.h"
#include "hal.h"
#include "ros/node_handle.h"

namespace ros {

template<typename T, T* io>
class ChibiOSHardware {
    public:
        void init() { }

        int read() {
            return chnGetTimeout(iostream, TIME_IMMEDIATE);
        }

        void write(uint8_t* data, int length) {
            chnWrite(iostream, data, length);
        }

        unsigned long time() {
            return chTimeNow();
        }

    protected:
        BaseChannel* iostream = reinterpret_cast<BaseChannel*>(io);
};

using NodeHandle = NodeHandle_<ChibiOSHardware<SerialUSBDriver, &SDU1>, 50, 50, 1024, 1024>;

} // namespace ros

#endif /* ROS_H */
