#ifndef CCAN_H
#define CCAN_H

#include <casa/casa.h>
#include <casa/blackboard.h>
#include <casa/rpc.h>

#ifdef __cplusplus
extern "C" {
#endif

// C Interface

#define ENOERR 0

typedef Casa_Errno_t CanErrorCode;
typedef Casa_BBObject_t CanBlackboard;

CanErrorCode can_read_blackboard(const CanBlackboard* object, void* dest);

#ifdef __cplusplus
}
#endif

#endif // CCAN_H
