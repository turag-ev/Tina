#ifndef ROS_LIB_TURAG_H_
#define ROS_LIB_TURAG_H_
#include <ros.h>
#include <tina++/container/thread_fifo.h>

namespace TURAG {

/// wrapper for ros::Publisher that automatically creates Msg instance
template<class MsgT>
class Publisher: public ros::Publisher {
public:
    constexpr Publisher(const char* name, int endpoint = rosserial_msgs::TopicInfo::ID_PUBLISHER):
        ros::Publisher(name, &msg, endpoint)
    { }

    constexpr Publisher(const char* ns, const char* name, int endpoint = rosserial_msgs::TopicInfo::ID_PUBLISHER):
        ros::Publisher(ns, name, &msg, endpoint)
    { }

    /**
     * @brief publishes internal msg
     */
    int publish() {
        return ros::Publisher::publish(&msg);
    }

    /// publishes msg given in argument
    using ros::Publisher::publish;

    MsgT msg;  ///> public, so that it can be used and the memory isn't wasted
};

template<class MsgT, size_t length>
class FifoPublisher :
        public Publisher<MsgT>,
        public ThreadFifo<MsgT, length> {
public:
    using MessageType = MsgT;
    using PublisherType = Publisher<MsgT>;
    using FifoType = ThreadFifo<MsgT, length>;
    static constexpr size_t LENGTH = length;

    // Export constructor
    // ThreadFifo is default-constructible
    using PublisherType::Publisher;

    // Export publisher API
    using PublisherType::getEndpointType;
    using PublisherType::topic_;
    using PublisherType::msg_;
    using PublisherType::id_;
    using PublisherType::nh_;
    using PublisherType::msg;

    // Export ThreadFifo API
    using FifoType::post;
    using FifoType::postUnique;
    using FifoType::empty;

    void fetch() {
        FifoType::fetch(&msg);
    }

    bool fetch(SystemTime time) {
        return FifoType::fetch(&msg, time);
    }

    int publish() {
        return PublisherType::publish();
    }
    
    void publish_pending() {
        while (fetch(SystemTime{0})) {
            publish();
        }
    }
};

} // namespace TURAG

#endif // ROS_LIB_TURAG_H_
