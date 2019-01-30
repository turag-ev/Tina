#ifndef ROS_LIB_TURAG_H_
#define ROS_LIB_TURAG_H_
#include <ros.h>

namespace TURAG {

/// wrapper for ros::Publisher that automatically creates Msg instance
template<class MsgT>
class Publisher: public ros::Publisher {
public:
    constexpr Publisher(const char* name, int endpoint = rosserial_msgs::TopicInfo::ID_PUBLISHER):
        ros::Publisher(name, &msg_, endpoint)
    { }

    constexpr Publisher(const char* ns, const char* name, int endpoint = rosserial_msgs::TopicInfo::ID_PUBLISHER):
        ros::Publisher(ns, name, &msg_, endpoint)
    { }

    /**
     * @brief publishes internal msg_
     */
    int publish() {
        return ros::Publisher::publish(&msg_);
    }

    // publishes msg given in argument
    using ros::Publisher::publish;

    MsgT msg_;  ///> public, so that it can be used and the memory isn't wasted
};

/// make ros classes available in TURAG namespace
template<class... T>
using Subscriber = ros::Subscriber<T...>;

/// Wrapper for less verbose service server declaration
template<typename SrvT>
using ServiceServer = ros::ServiceServer<typename SrvT::Request, typename SrvT::Response>;

typedef ros::NodeHandle NodeHandle;

} // namespace TURAG

#endif // ROS_LIB_TURAG_H_
