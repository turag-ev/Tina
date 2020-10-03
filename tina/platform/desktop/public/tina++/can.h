/*
 * cansc.h
 *
 *  Created on: 18.07.2012
 *      Author: richard
 */

#ifndef PLATFORM_DESKTOP_PUBLIC_TINAPP_CAN_H
#define PLATFORM_DESKTOP_PUBLIC_TINAPP_CAN_H

#include <tina++/tina.h>
#include <tina++/helper/init.h>

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// CAN

namespace CAN {

typedef uint FuncId;
typedef uint Id;
typedef uint ErrorCode;

struct Blackboard {
    Id id;
    Id owner;
};

/// RPC handler function type
typedef uint64_t (*RpcHandler)(uint64_t param);

/// Call a CASA RPC
/// \param remote_id id of the reciver
/// \param func_id   id of the rpc
/// \param param     64bit of data
/// \returns return value of rpc as 64bit of data
uint64_t call(Id remote_id, FuncId func_id, uint64_t param, ErrorCode* error_res = nullptr);

// CASA_RPC_BIND C++ replacement
extern RpcHandler sc_can_rpc_table[];
#define CAN_RPC_BIND(function, id) TURAG_INIT_VAR(TURAG::CAN::sc_can_rpc_table[id], function);

extern RpcHandler mc_can_rpc_table[];
#define CAN_MC_RPC_BIND(function, id) TURAG_INIT_VAR(TURAG::CAN::mc_can_rpc_table[id], function);

extern RpcHandler ui_can_rpc_table[];
#define CAN_UI_RPC_BIND(function, id) TURAG_INIT_VAR(TURAG::CAN::ui_can_rpc_table[id], function);

#define CAN_RPC_HANDLER_DEF(id) uint64_t Casa_RpcHandler_##id (uint64_t)

// CASA_RPC_HANDLER C++ replacement
#define CAN_RPC_HANDLER(id, param) \
  uint64_t Casa_RpcHandler_##id (uint64_t); \
  CAN_RPC_BIND(Casa_RpcHandler_##id, id) \
  uint64_t Casa_RpcHandler_##id (uint64_t param)

#define CAN_MC_RPC_HANDLER(id, param) \
  uint64_t Casa_MCRpcHandler_##id (uint64_t); \
  CAN_MC_RPC_BIND(Casa_MCRpcHandler_##id, id) \
  uint64_t Casa_MCRpcHandler_##id (uint64_t param)

#define CAN_UI_RPC_HANDLER(id, param) \
  uint64_t Casa_UIRpcHandler_##id (uint64_t); \
  CAN_UI_RPC_BIND(Casa_UIRpcHandler_##id, id) \
  uint64_t Casa_UIRpcHandler_##id (uint64_t param)

// CASA_BBOBJECT C++ replacement
#define CAN_BBOBJECT(name, objectId, objectOwner, dataSize) \
  extern "C" const TURAG::CAN::Blackboard name = { \
    id: objectId, \
    owner: objectOwner \
  }

/// Read value from CASA blackboard
/// \param object a CASA blackboard
/// \param dest   destination of blackboard value
ErrorCode read_blackboard(const Blackboard* object, pointer dest);

/// Setze Funktion, die aufrufen wird, wenn neuer Wert in Blackboard geschreiben wurde
/// \param object Blackboard
/// \param callback aufzurufende Funktion
/// \param data Der Funktion mit zu übergebende Benutzerdaten
bool set_blackboard_notify_callback(const Blackboard* object,
                                    BlackboardCallback callback,
                                    uint64_t data);

} // namespace CAN

} // namespace TURAG

#endif // PLATFORM_DESKTOP_PUBLIC_TINAPP_CAN_H
