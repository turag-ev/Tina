#ifndef TINAPP_ECOS_CAN_H
#define TINAPP_ECOS_CAN_H

#include <turag/casa.h>
#include <tina++/tina.h>
#include <tina/can.h>

namespace TURAG {

namespace CAN {

typedef Casa_FuncId_t FuncId;
typedef Casa_Id_t Id;
typedef Cyg_ErrNo ErrorCode;
typedef Casa_BBObject_t Blackboard;

/// RPC handler function type
typedef uint64_t (*RpcHandler)(uint64_t param);

constexpr ErrorCode NoError = -ENOERR;
constexpr ErrorCode Invalid = -EINVAL;
constexpr ErrorCode Again = -EAGAIN;
constexpr ErrorCode NoEntry = -ENOENT;



/// Call a CASA RPC
/// \param remote_id id of the reciver
/// \param func_id   id of the rpc
/// \param param     64bit of data
/// \returns return value of rpc as 64bit of data
uint64_t call(Id remote_id, FuncId func_id, uint64_t param, ErrorCode* error_res = nullptr);


/// Read value from CASA blackboard
/// \param object a CASA blackboard
/// \param dest   destination of blackboard value
ErrorCode read_blackboard(const Blackboard* object, pointer dest);

} // namespace CAN

} // namespace TURAG

#endif // TINAPP_ECOS_CAN_H
