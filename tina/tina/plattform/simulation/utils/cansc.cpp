/*
 * cansc.cpp
 *
 *  Created on: 18.07.2012
 *      Author: richard
 */

#define LOG_SOURCE "C"
#include "utils/debug.h"

#include "cansc.h"
#include "comm/id.h"

namespace SystemControl {

////////////////////////////////////////////////////////////////////////////////
// CAN

namespace CAN {

// CASA replacement
const size_t can_rpc_table_size = 62;

RpcHandler sc_can_rpc_table[can_rpc_table_size] = {0};
RpcHandler mc_can_rpc_table[can_rpc_table_size] = {0};
RpcHandler ui_can_rpc_table[can_rpc_table_size] = {0};

_always_inline uint64_t
call_rpc(RpcHandler table[], FuncId func_id, uint64_t param) {
  const RpcHandler& handler = table[func_id];
  return (handler != 0) ? handler(param) : 0;
}

extern void read_blackboard_request(Id id, Id owner, pointer dest);
extern void write_blackboard_request(Id id, Id owner, pointer src);

uint64_t call(Id remote_id, FuncId func_id, uint64_t param, ErrorCode* error_res) {
  if (error_res) *error_res = 0;

  switch (remote_id) {
  case CASA_ID_ALL:
    call_rpc(sc_can_rpc_table, func_id, param);
    call_rpc(mc_can_rpc_table, func_id, param);
    call_rpc(ui_can_rpc_table, func_id, param);
    return 0;

  case CASA_ID_SC:
    return call_rpc(sc_can_rpc_table, func_id, param);

  case CASA_ID_MC:
    return call_rpc(mc_can_rpc_table, func_id, param);

  default:
    return call_rpc(ui_can_rpc_table, func_id, param);
  }
}

__attribute__((hot))
ErrorCode read_blackboard(const Blackboard *object, pointer dest) {
  read_blackboard_request(object->id, object->owner, dest);
  return 0;
}

// C Interface

extern "C" __attribute__((hot))
ErrorCode can_read_blackboard(const Blackboard* object, pointer dest) {
  read_blackboard_request(object->id, object->owner, dest);
  return 0;
}

} /* namespace CAN */

} /* namespace SystemControl */
