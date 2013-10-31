#ifndef CCAN_H
#define CCAN_H

#include <turag/casa.h>

#ifdef __cplusplus
extern "C" {
#endif

// C Interface

#define ENOERR 0

typedef Cyg_ErrNo TuragCanErrorCode;
typedef Casa_BBObject_t TuragCanBlackboard;

TuragCanErrorCode turag_can_read_blackboard(const TuragCanBlackboard* object, void* dest);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CCAN_H
