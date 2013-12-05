#ifndef TINA_CHIBIOS_CAN_H
#define TINA_CHIBIOS_CAN_H

#include <casa/casa.h>
#include <casa/blackboard.h>
#include <casa/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

// C Interface

#define ENOERR 0

typedef Casa_Errno_t TuragCanErrorCode;
typedef Casa_BBObject_t TuragCanBlackboard;

TuragCanErrorCode turag_can_read_blackboard(const TuragCanBlackboard* object, void* dest);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_CHIBIOS_CAN_H
