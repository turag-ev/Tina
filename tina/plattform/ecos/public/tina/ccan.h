#ifndef CCAN_H
#define CCAN_H

#include <turag/casa.h>

#ifdef __cplusplus
extern "C" {
#endif

// C Interface

#define ENOERR 0

typedef Cyg_ErrNo CanErrorCode;
typedef Casa_BBObject_t CanBlackboard;

CanErrorCode can_read_blackboard(const CanBlackboard* object, void* dest);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CCAN_H
