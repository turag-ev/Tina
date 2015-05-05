#ifndef TINAPP_CHIBIOS_CAN_H
#define TINAPP_CHIBIOS_CAN_H

#include <casa/casa.h>
#include <casa/blackboard.h>
#include <casa/rpc.h>

#include <tina++/tina.h>
#include <tina/can.h>


namespace TURAG {

namespace CAN {

typedef Casa_FuncId_t FuncId;
typedef Casa_Id_t Id;
typedef Casa_Errno_t ErrorCode;
typedef Casa_BBObject_t Blackboard;

/// RPC handler function type
typedef uint64_t (*RpcHandler)(uint64_t param);

/// Blackboard callback function type
typedef Casa_BBCallback_t BlackboardCallback;

constexpr ErrorCode NoError = -CASA_ENOERR;
constexpr ErrorCode Invalid = -CASA_EINVAL;
constexpr ErrorCode Again = -CASA_EAGAIN;
constexpr ErrorCode NoEntry = -CASA_ENOENT;


/// Call a CASA RPC
/// \param remote_id id of the reciver
/// \param func_id   id of the rpc
/// \param param     64bit of data
/// \param error_res if given, after the call the variable will hold a return value
/// \returns return value of rpc as 64bit of data
uint64_t call(Id remote_id, FuncId func_id, uint64_t param, ErrorCode* error_res = nullptr);


/// Read value from CASA blackboard
/// \param object a CASA blackboard
/// \param dest   destination of blackboard value
/// \param error  destination of error value or \a nullptr
/// \returns \a true if opertation was successful
bool readBlackboard(const Blackboard *object, pointer dest, ErrorCode* error = nullptr);

/// \copydoc turag_set_blackboard_notify_callback
inline
bool set_blackboard_notify_callback(const Blackboard* object,
                                    TuragBlackboardCallback callback,
                                    uint64_t callbackData)
{
    return object && (Casa_BBSetCallback(object->id, callback, callbackData) == -CASA_ENOERR);
}

} // namespace CAN

} // namespace TURAG

#endif // TINAPP_CHIBIOS_CAN_H
