#ifndef TINA_ECOS_CAN_H
#define TINA_ECOS_CAN_H

#include <turag/casa.h>
#include <tina/tina.h>

#ifdef __cplusplus
extern "C" {
#endif

// C Interface

#define TURAG_CASA_ENOERR (-ENOERR)
#define TURAG_CASA_EINVAL (-EINVAL)
#define TURAG_CASA_EAGAIN (-EAGAIN)
#define TURAG_CASA_ENOENT (-ENOENT)

typedef Cyg_ErrNo TuragCanErrorCode;
typedef Casa_BBObject_t TuragCanBlackboard;
typedef Casa_Id_t TuragCanId;
typedef Casa_FuncId_t TuragCanFuncId;

TuragCanErrorCode turag_can_read_blackboard(const TuragCanBlackboard* object, void* dest);

/// Call a CASA RPC
/// \param remote_id id of the reciver
/// \param func_id   id of the rpc
/// \param param     64bit of data
/// \returns return value of rpc as 64bit of data
uint64_t turag_can_call(TuragCanId remote_id, TuragCanFuncId func_id, uint64_t param, TuragCanErrorCode* error_res);

// CASA_RPC_BIND C replacement
#define CAN_RPC_BIND(function, id) \
  Casa_RpcBinding_t _Casa_RpcHandlerTableEntry_##id CYG_HAL_TABLE_ENTRY(Casa_RpcHandlers) = { \
    funcId: id, handler: function \
  }

#define CAN_RPC_HANDLER_DEF(id) uint64_t Casa_RpcHandler_##id (uint64_t)

// CASA_RPC_HANDLER C replacement
#define CAN_RPC_HANDLER(id, param) \
  uint64_t Casa_RpcHandler_##id (uint64_t); \
  CAN_RPC_BIND(Casa_RpcHandler_##id, id); \
  uint64_t Casa_RpcHandler_##id (uint64_t param)

// CASA_BBOBJECT C replacement
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


#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_ECOS_CAN_H
