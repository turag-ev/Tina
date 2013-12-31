#ifndef TINA_ECOS_FLOAT_H_
#define TINA_ECOS_FLOAT_H_

#include <tina/tina.h>

#ifdef __cplusplus
extern "C" {
#endif

float acosf(float);
float asinf(float);
float atanf(float);
float atan2f(float,float);
float ceilf(float);
float copysignf(float,float);
float cosf(float);
float fabsf(float);
float floorf(float);
float hypotf(float,float);
float powf(float,float);
float sinf(float);
float sqrtf(float);
float tanf(float);
long int lroundf(float);
float scalbnf(float,int);

TURAG_INLINE float fmaxf(float x, float y) {
  return (x > y) ? x : y;
}

TURAG_INLINE float fminf(float x, float y) {
  return (x < y) ? x : y;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* TINA_ECOS_FLOAT_H_ */
