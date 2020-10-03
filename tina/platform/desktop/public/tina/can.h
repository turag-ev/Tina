#ifndef PLATFORM_DESKTOP_PUBLIC_TINA_CAN_H
#define PLATFORM_DESKTOP_PUBLIC_TINA_CAN_H

// C Interface

#ifdef __cplusplus
extern "C" {
#endif

#define ENOERR 0

typedef unsigned TuragCanErrorCode;
typedef unsigned TuragCanId;

typedef struct {
  TuragCanId id;
  TuragCanId owner;
} TuragCanBlackboard;

TuragCanErrorCode turag_can_read_blackboard(const TuragCanBlackboard* object, void* dest);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PLATFORM_DESKTOP_PUBLIC_TINA_CAN_H
