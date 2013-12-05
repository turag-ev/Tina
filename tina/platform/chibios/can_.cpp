#include <cstdio>
#define LOG_SOURCE "C"
#include <tina/debug.h>

extern "C" {
#include <casa/casa.h>
#include <casa/blackboard.h>
#include <casa/rpc.h>
}
#include "public/tina++/can.h"
#include "public/tina/can.h"
#include "xserial.h"

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
        if (errorcnt++ <= 1) {
            errorf("CASA RPC (Id: %d / Func: %d) failed with error code: 0x%.8x", remote_id, func_id, error);
            if (errorcnt == 2)
                errorf("(omitting the following 199 CASARPC messages)");
        } else if (errorcnt == 200) {
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
        if (errorcnt++ <= 1) {
            errorf("CASA BlackBoard read failed with error code: 0x%.8x", error);
            if (errorcnt == 2)
                errorf("(omitting the following 199 CASABB messages)");
        } else if (errorcnt == 200) {
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

} /* namespace CAN */

} /* namespace TURAG */
