#ifndef TINA_MATH_H
#define TINA_MATH_H

#include <math.h>
#include <tina/tina.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef M_E
# define M_E            2.7182818284590452354   /* e */
#endif

#ifndef M_LOG2E
# define M_LOG2E        1.4426950408889634074   /* log_2 e */
#endif 

#ifndef M_LOG10E
# define M_LOG10E       0.43429448190325182765  /* log_10 e */
#endif

#ifndef M_LN2
# define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif

#ifndef M_LN10
# define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

#ifndef M_PI_2
# define M_PI_2         1.57079632679489661923  /* pi/2 */
#endif

#ifndef M_PI_4
# define M_PI_4         0.78539816339744830962  /* pi/4 */
#endif

#ifndef M_1_PI
# define M_1_PI         0.31830988618379067154  /* 1/pi */
#endif

#ifndef M_2_PI
# define M_2_PI         0.63661977236758134308  /* 2/pi */
#endif

#ifndef M_2_SQRTPI
# define M_2_SQRTPI     1.12837916709551257390  /* 2/sqrt(pi) */
#endif

#ifndef M_SQRT2
# define M_SQRT2        1.41421356237309504880  /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
# define M_SQRT1_2      0.70710678118654752440  /* 1/sqrt(2) */
#endif

#ifndef M_SQRT3
# define M_SQRT3        1.73205080756887729352  /* sqrt(3) */
#endif

#ifndef M_SQRT3_2
# define M_SQRT3_2      0.86602540378443864676  /* sqrt(3)/2 */
#endif

#ifndef NAN
#define NAN                     (0.0f / 0.0f)
#endif

#ifndef INF
#define INF                     (1.0f / 0.0f)
#endif

// float constants

#define M_Ef            2.7182818284590452354f   /* e */
#define M_LOG2Ef        1.4426950408889634074f  /* log_2 e */
#define M_LOG10Ef       0.43429448190325182765f  /* log_10 e */
#define M_LN2f          0.69314718055994530942f  /* log_e 2 */
#define M_LN10f         2.30258509299404568402f  /* log_e 10 */
#define M_PIf           3.14159265358979323846f  /* pi */
#define M_PI_2f         1.57079632679489661923f  /* pi/2 */
#define M_PI_4f         0.78539816339744830962f  /* pi/4 */
#define M_1_PIf         0.31830988618379067154f  /* 1/pi */
#define M_2_PIf         0.63661977236758134308f  /* 2/pi */
#define M_2_SQRTPIf     1.12837916709551257390f  /* 2/sqrt(pi) */
#define M_SQRT2f        1.41421356237309504880f  /* sqrt(2) */
#define M_SQRT1_2f      0.70710678118654752440f  /* 1/sqrt(2) */
#define M_SQRT3f        1.73205080756887729352f  /* sqrt(3) */
#define M_SQRT3_2f      0.86602540378443864676f  /* sqrt(3)/2 */

// float functions constants

#ifdef ECOS
#include <tina/float.h>
#endif

// extra math functions
#define mRadToRad(x)        ((x) * 0.001f)
#define radTomRad(x)        ((x) * 1000.0f)
#define mmTom(x)            ((x) * 0.001f)
#define mTomm(x)            ((x) * 1000.0f)

#define degToRad(x)  	((x) * M_PIf / 180.0f)
#define radToDeg(x)     ((x) * 180.0f / M_PIf)

#define square(x)     	((x) * (x))
#define cubic(x)		((x)*(x)*(x))




TURAG_INLINE float sqrf(float x) {
  return x * x;
}

TURAG_INLINE float cbcf(float x) {
  return x * x * x;
}

TURAG_INLINE float sgnf(float x) {
  return (x==0.0f) ? 0.0f :
                   ((x > 0.0f) ? 1.0f : -1.0f);
}

TURAG_INLINE int sgn(int x) {
  return (x==0) ? 0 :
                   ((x > 0) ? 1 : -1);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TINA_MATH_H
