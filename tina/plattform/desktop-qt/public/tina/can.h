#ifndef TINA_SIM_CAN_H
#define TINA_SIM_CAN_H

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

#endif // TINA_SIM_CAN_H
