#ifndef CCAN_H
#define CCAN_H

// C Interface

#ifdef __cplusplus
extern "C" {
#endif

#define ENOERR 0

typedef unsigned CanErrorCode;
typedef unsigned CanId;

typedef struct {
  CanId id;
  CanId owner;
} CanBlackboard;

CanErrorCode can_read_blackboard(const CanBlackboard* object, void* dest);

#ifdef __cplusplus
}
#endif

#endif // CCAN_H
