#ifndef ROS_H
#define ROS_H

#include "ch.h"
#include "hal.h"
#include "ros/node_handle.h"

#ifdef __cplusplus
extern "C" {
#endif

extern SerialUSBDriver SDU1;

namespace ros {

class ChibiOSHardware {
    public:
        ChibiOSHardware(BaseChannel* io)
        {
            iostream = io;
        }
        ChibiOSHardware()
        {
            iostream = (BaseChannel *)&SDU1;
        }

        void init()
        {
        }

        int read()
        {
            return chnGetTimeout(iostream, TIME_IMMEDIATE);
        };

        void write(uint8_t* data, int length)
        {
            chnWrite(iostream, data, length);
        }

        unsigned long time()
        {
            return chVTGetSystemTimeX();
        }

    protected:
        BaseChannel* iostream;
};

typedef NodeHandle_<ChibiOSHardware, 50, 50, 1024, 1024> NodeHandle;

} // namespace ros

#ifdef __cplusplus
}
#endif

#endif /* ROS_H */
