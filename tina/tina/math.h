#ifndef TINA_MATH_H
#define TINA_MATH_H

#include <math.h>


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
#define NAN                     (0.0 / 0.0)
#endif

#ifndef INF
#define INF                     (1.0 / 0.0)
#endif



#define degToRad(deg)	((deg) * M_PI / 180.0)
#define radToDeg(rad)	((rad) * 180.0 / M_PI)
#define square(x)		(((float)x)*((float)x))
#define cubic(x)		(((float)x)*((float)x)*((float)x))
#define L2_Norm(x,y)	((float)sqrt(square(x)+square(y)))
#define sign(x)         (((float)(x))==0.0f ? 0.0f : (((float)(x)) > 0.0f ? 1.0f : -1.0f))
#define DOT(v1,v2) 		(((float)((v1).x)*(float)((v2).x)) + ((float)((v1).y)*(float)((v2).y)))




#endif // TINA_MATH_H
