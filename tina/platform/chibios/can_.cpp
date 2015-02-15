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

ErrorCode read_blackboard(const Blackboard *object, pointer dest) {
    static unsigned errorcnt = 0;
    uint64_t timestamp;

    ErrorCode error = Casa_BBRead((Casa_BBObject_t*)object, dest, (systime_t*)&timestamp);

    if (error != -CASA_ENOERR) {
        if (errorcnt++ <= 4) {
            turag_errorf("CASA BlackBoard read failed with error code: %d (0x%.8x)", error, error);
            if (errorcnt == 5)
                turag_errorf("(omitting the following 5k CASABB messages)");
        } else if (errorcnt == 5000) {
            errorcnt = 0;
        }
    } else {
        errorcnt = 0;
    }

    return error;
}

// C Interface

extern "C"
ErrorCode turag_can_read_blackboard(const Blackboard* object, pointer dest) {
    return CAN::read_blackboard(object, dest);
}

extern "C"
uint64_t turag_can_call(Id remote_id, FuncId func_id, uint64_t param, ErrorCode* error_res) {
	return call(remote_id, func_id, param, error_res);
}

} /* namespace CAN */

} /* namespace TURAG */
