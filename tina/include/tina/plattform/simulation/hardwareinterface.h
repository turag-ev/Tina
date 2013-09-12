#ifndef SIM_HARDWARE_H
#define SIM_HARDWARE_H

#include <utils/types.h>

// turag robot_io

////////////////////////////////////////////////////////////////////////////////
// Servo

#define Servo_Enable(index)	Servo_SetEnabled(index, true)
#define Servo_Disable(index)	Servo_SetEnabled(index, false)

inline void Servo_SetEnabled(unsigned, bool) {}
inline bool Servo_IsEnabled(unsigned) {return true;}
inline void Servo_SetPos(unsigned, uint32_t) {}
inline void Servo_SetRange(unsigned, uint32_t, uint32_t) {}
inline void Servo_SetRamp(unsigned, uint32_t, uint32_t) {}

////////////////////////////////////////////////////////////////////////////////
// Pin

typedef enum{
  Pin_Mode_Invalid = 0,
  Pin_Mode_DIn,
  Pin_Mode_DOut,
  Pin_Mode_AIn,
  Pin_Mode_AOut
} Pin_Mode_t;

#define PIN_DOUTFLAG_OPENDRAIN	(1 << 0)
#define PIN_DINFLAG_PULLUP	(1 << 8)
#define PIN_DINFLAG_PULLDOWN	(1 << 9)

// Pin_Mode_t and all possible flags are defined before including the HAL header
inline bool Pin_Setup(unsigned, Pin_Mode_t, uint32_t, int32_t) { return true; }
inline void Pin_DOutSetLevel(unsigned, bool) {}
inline bool Pin_DOutGetLevel(unsigned) { return true; }
#define Pin_DOutSetFlags(index, flags) \
        Pin_Setup(index, Pin_Mode_DOut, flags, Pin_DOutGetLevel(index))

inline bool Pin_DInGetLevel(unsigned, bool *) {return true;}
#define Pin_DInSetFlags(index, flags) \
        Pin_Setup(index, Pin_Mode_DIn, flags, 0);

inline bool Pin_AInGetVoltage(unsigned, int32_t *) {return false;}
#define Pin_AInSetReference(index, reference) \
        Pin_Setup(index, Pin_Mode_AIn, Pin_AInGetFlags(index), reference)
#define Pin_AInSetFlags(index, flags) \
        Pin_Setup(index, Pin_Mode_AIn, flags, Pin_AInGetReference(index))

////////////////////////////////////////////////////////////////////////////////
// LED

#define COLOR_OFF     0
#define COLOR_RED     1
#define COLOR_GREEN     2
#define COLOR_BLUE      4
#define COLOR_YELLOW    (COLOR_RED | COLOR_GREEN)
#define COLOR_MAGENTA   (COLOR_RED | COLOR_BLUE)
#define COLOR_CYAN      (COLOR_GREEN | COLOR_BLUE)
#define COLOR_WHITE     (COLOR_RED | COLOR_GREEN | COLOR_BLUE)
inline void Led_SetColor(unsigned) {}

#endif // SIM_HARDWARE_H
