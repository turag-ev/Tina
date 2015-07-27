#include <cstdio>
#define TURAG_DEBUG_LOG_SOURCE "C"
#include <tina/debug.h>

#include <board_turag.h>
#include <os_turag.h>
#include "public/tina++/can.h"
#include "public/tina/can.h"

namespace TURAG {

////////////////////////////////////////////////////////////////////////////////
// CAN

namespace CAN {

uint64_t call(Id remote_id, FuncId func_id, uint64_t param, ErrorCode* error_res) {
    static unsigned errorcnt = 0;
    uint64_t result = 0;

    // RpcInvoke tries to send the message 3 times
    ErrorCode error = Casa_RpcInvoke(remote_id, func_id, param, &result);

    // with unconnected CAN bus this does spam-a-lot
    if (error != -CASA_ENOERR) {
        if (errorcnt++ <= 4) {
            turag_errorf("CASA RPC (Id: %d / Func: %d) failed with error code: %d (0x%.8x)", remote_id, func_id, error, error);
            if (errorcnt == 5)
                turag_errorf("(omitting the following 500 CASARPC messages)");
        } else if (errorcnt == 500) {
            errorcnt = 0;
        }
    } else {
        errorcnt = 0;
    }

    if (error_res)
        *error_res = error;

    return result;
}

bool readBlackboard(const Blackboard *object, void* dest, ErrorCode* error)
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
bool turag_can_read_blackboard(const Blackboard* object, void* dest, TuragCanErrorCode* error_res) {
    return CAN::readBlackboard(object, dest, error_res);
}

extern "C"
uint64_t turag_can_call(Id remote_id, FuncId func_id, uint64_t param, TuragCanErrorCode* error_res) {
	return call(remote_id, func_id, param, error_res);
}

} /* namespace CAN */

} /* namespace TURAG */
