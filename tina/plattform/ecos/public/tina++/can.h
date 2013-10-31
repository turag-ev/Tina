#ifndef CAN_H_
#define CAN_H_

#include <turag/casa.h>
#include <tina++/tina.h>

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// CAN

namespace CAN {

typedef Casa_FuncId_t FuncId;
typedef Casa_Id_t Id;
typedef Cyg_ErrNo ErrorCode;
typedef Casa_BBObject_t Blackboard;

/// RPC handler function type
typedef uint64_t (*RpcHandler)(uint64_t param);

/// Call a CASA RPC
/// \param remote_id id of the reciver
/// \param func_id   id of the rpc
/// \param param     64bit of data
/// \returns return value of rpc as 64bit of data
uint64_t call(Id remote_id, FuncId func_id, uint64_t param, ErrorCode* error_res = nullptr);

// CASA_RPC_BIND C++ replacement
#define CAN_RPC_BIND(function, id) \
  Casa_RpcBinding_t _Casa_RpcHandlerTableEntry_##id CYG_HAL_TABLE_ENTRY(Casa_RpcHandlers) = { \
    funcId: id, handler: function \
  }

#define CAN_RPC_HANDLER_DEF(id) uint64_t Casa_RpcHandler_##id (uint64_t)

// CASA_RPC_HANDLER C++ replacement
#define CAN_RPC_HANDLER(id, param) \
  uint64_t Casa_RpcHandler_##id (uint64_t); \
  CAN_RPC_BIND(Casa_RpcHandler_##id, id); \
  uint64_t Casa_RpcHandler_##id (uint64_t param)

// CASA_BBOBJECT C++ replacement
#define CAN_BBOBJECT(name, objectId, objectOwner, dataSize) \
  static struct { \
    Casa_BBObject_RuntimeData_t rtData; \
    char *buffers[2][dataSize]; \
  } name##_runtime_data; \
  extern "C" const Casa_BBObject_t name CYG_HAL_TABLE_ENTRY(Casa_BlackboardObjects) = { \
    id: objectId, \
    owner: objectOwner, \
    size: dataSize, \
    rtData: &name##_runtime_data.rtData \
  };

/// Read value from CASA blackboard
/// \param object a CASA blackboard
/// \param dest   destination of blackboard value
ErrorCode read_blackboard(const Blackboard* object, pointer dest);

} /* namespace CAN */

} // namespace TURAG

#endif /* CAN_H_ */
