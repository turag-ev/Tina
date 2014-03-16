#ifndef TINA_CHIBIOS_CAN_H
#define TINA_CHIBIOS_CAN_H

#include <casa/casa.h>
#include <casa/blackboard.h>
#include <casa/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

// C Interface

#define TURAG_CASA_ENOERR (-CASA_ENOERR)
#define TURAG_CASA_EINVAL (-CASA_EINVAL)
#define TURAG_CASA_EAGAIN (-CASA_EAGAIN)
#define TURAG_CASA_ENOENT (-CASA_ENOENT)

typedef Casa_Errno_t TuragCanErrorCode;
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

// CASA_RPC_BIND C++ replacement
#define CAN_RPC_BIND(function, id) \
  Casa_RpcBinding_t _Casa_RpcHandlerTableEntry_##id BANANA_TABLE_ENTRY(Casa_RpcHandlers) = { \
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
  extern "C" const Casa_BBObject_t name BANANA_TABLE_ENTRY(Casa_BlackboardObjects) = { \
    id: objectId, \
    owner: objectOwner, \
    size: dataSize, \
    timestamp: 0, \
    lock: BinarySemaphore(), \
    writer: 0, \
    byteIndex: 0, \
    lastWrite: 0, \
    /*callback: 0,*/ \
    /*callbackData: 0,*/ \
    dataBuffer: NULL, \
    commBuffer: NULL, \
    buffer: { 0 } \
  }

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_CHIBIOS_CAN_H
