/*
 * cansc.cpp
 *
 *  Created on: 18.07.2012
 *      Author: richard
 */

#include <cstdio>

#define TURAG_DEBUG_LOG_SOURCE "C"
#include <tina/debug/print.h>

#include "../public/tina++/can.h"


namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// CAN

namespace CAN {

uint64_t call(Id remote_id, FuncId func_id, uint64_t param, ErrorCode* error_res) {
  uint64_t result = 0;
  uint i = 0;

  ErrorCode error = Casa_RpcInvoke(remote_id, func_id, param, &result);
  while (error && i < 3) {
    turag_warningf("CASA RPC (remote: %d func: %d) failed with error code: %d", remote_id, func_id, error);
    error = Casa_RpcInvoke(remote_id, func_id, param, &result);
    i++;
  }

  // transmission failed
  if (i == 3) {
    turag_errorf("CASA RPC (remote: %d func: %d) failed with error code: %d", remote_id, func_id, error);
    if (error_res) *error_res = error;
  } else if (error_res) {
    *error_res = 0;
  }

  return result;
}

__attribute__((hot))
ErrorCode read_blackboard(const Blackboard *object, pointer dest) {
  uint64_t timestamp;
  
  ErrorCode error = Casa_BBRead(object, dest, &timestamp);
  if (error) {
    turag_errorf("CASA BlackBoard read failed with error code: %d", error);
  }
  return error;
}


// C Interface

extern "C" __attribute__((hot))
ErrorCode turag_can_read_blackboard(const Blackboard* object, pointer dest) {
	return read_blackboard(object, dest);
}

extern "C"
uint64_t turag_can_call(Id remote_id, FuncId func_id, uint64_t param, ErrorCode* error_res) {
	return call(remote_id, func_id, param, error_res);
}


} /* namespace CAN */

} // namespace TURAG


