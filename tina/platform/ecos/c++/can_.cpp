/*
 * cansc.cpp
 *
 *  Created on: 18.07.2012
 *      Author: richard
 */

#include <cstdio>

#define LOG_SOURCE "C"
#include <tina/debug.h>

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
    warningf("CASA RPC (remote: %d func: %d) failed with error code: %d", remote_id, func_id, error);
    error = Casa_RpcInvoke(remote_id, func_id, param, &result);
    i++;
  }

  // transmission failed
  if (i == 3) {
    errorf("CASA RPC (remote: %d func: %d) failed with error code: %d", remote_id, func_id, error);
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
    errorf("CASA BlackBoard read failed with error code: %d", error);
  }
  return error;
}

// C Interface

extern "C" __attribute__((hot))
ErrorCode turag_can_read_blackboard(const Blackboard* object, pointer dest) {
  uint64_t timestamp;
  
  ErrorCode error = Casa_BBRead(object, dest, &timestamp);
  if (error) {
    errorf("CASA BlackBoard read failed with error code: %d", error);
  }
  return error;
}

} /* namespace CAN */

} // namespace TURAG

