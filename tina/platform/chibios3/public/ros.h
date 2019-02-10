#ifndef ROS_H
#define ROS_H

#include "ch.h"
#include "hal.h"
#include "ros/node_handle.h"

namespace ros {

class ChibiOSHardware {
    public:
        void init(char* io) {
            iostream = reinterpret_cast<BaseChannel *>(io);
        }

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
        BaseChannel* iostream = nullptr;
};

class NodeHandle : public NodeHandle_<ChibiOSHardware, 50, 50, 1024, 1024> {
public:
    void initNode(BaseChannel* io) {
        NodeHandle_::initNode(reinterpret_cast<char *>(io));
    }
private:
    using NodeHandle_::initNode;
};

} // namespace ros

#endif /* ROS_H */
