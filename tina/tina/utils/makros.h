#ifndef COMMON_UTIL_MAKROS_H
#define COMMON_UTIL_MAKROS_H

static inline float fmaxf(float value1, float value2){
  if(value1 > value2)
    return value1;
  else
    return value2;
}

static inline double fmax(double value1, double value2){
  if(value1 > value2)
    return value1;
  else
    return value2;
}

static inline long double fmaxl(long double value1, long double value2){
  if(value1 > value2)
    return value1;
  else
    return value2;
}

static inline float fminf(float value1, float value2){
  if(value1 < value2)
    return value1;
  else
    return value2;
}

static inline double fmin(double value1, double value2){
  if(value1 < value2)
    return value1;
  else
    return value2;
}

static inline long double fminl(long double value1, long double value2){
  if(value1 < value2)
    return value1;
  else
    return value2;
}


static inline uint8_t highbyte(uint16_t halfword) {
	return (halfword >> 8) & 0xFF;
}

static inline uint8_t lowbyte(uint16_t halfword) {
	return halfword & 0xFF;
}

static inline uint16_t halfword(uint8_t highbyte, uint8_t lowbyte) {
	return ((uint16_t)highbyte << 8) + lowbyte;
}

#endif // COMMON_UTIL_MAKROS_H
