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
bool readBlackboard(const Blackboard *object, pointer dest, ErrorCode* error)
{
    static unsigned errorcnt = 0;
    systime_t timestamp;

    if (!object) {
        turag_critical("read_blackboard called with nullptr!");
        return -1;
    }

    ErrorCode e = Casa_BBRead((Casa_BBObject_t*)object, dest, &timestamp);
    
    if (error != nullptr)
      *error = e;

    if (e != -CASA_ENOERR) {
        if (errorcnt++ <= 4) {
            turag_errorf("CASA BlackBoard read failed with error code: objId %d, objOwner 0x%x, err %d (0x%.8x)",
                object->id, object->owner, e, e);
            if (errorcnt == 5)
                turag_errorf("(omitting the following 5k CASABB messages)");
        } else if (errorcnt == 5000) {
            errorcnt = 0;
        }        
        return false;
    }
    
    errorcnt = 0;
    return true;
}


// C Interface

extern "C"
bool turag_can_read_blackboard(const Blackboard* object, pointer dest, TuragCanErrorCode* error_res) {
    return CAN::readBlackboard(object, dest, error_res);
}

extern "C"
uint64_t turag_can_call(Id remote_id, FuncId func_id, uint64_t param, TuragCanErrorCode* error_res) {
	return call(remote_id, func_id, param, error_res);
}


} /* namespace CAN */

} // namespace TURAG


