/*
 * cansc.h
 *
 *  Created on: 18.07.2012
 *      Author: richard
 */

#ifndef CAN_H_
#define CAN_H_

extern "C" {
#include <casa/casa.h>
#include <casa/blackboard.h>
#include <casa/rpc.h>
}
#include "utils/types.h"
#include "utils/extra/macro-helper.h"

namespace SystemControl {

////////////////////////////////////////////////////////////////////////////////
// CAN

namespace CAN {

typedef Casa_FuncId_t FuncId;
typedef Casa_Id_t Id;
typedef Casa_Errno_t ErrorCode;
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
  };

/// Read value from CASA blackboard
/// \param object a CASA blackboard
/// \param dest   destination of blackboard value
ErrorCode read_blackboard(const Blackboard* object, pointer dest);

} /* namespace CAN */

} /* namespace SystemControl */

#endif /* CAN_H_ */
