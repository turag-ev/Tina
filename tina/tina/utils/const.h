#ifndef COMMON_UTIL_CONST_H
#define COMMON_UTIL_CONST_H

#define degToRad(deg)	((deg) * M_PI / 180.0)
#define radToDeg(rad)	((rad) * 180.0 / M_PI)
#define square(x)		(((float)x)*((float)x))
#define cubic(x)		(((float)x)*((float)x)*((float)x))
#define L2_Norm(x,y)	((float)sqrt(square(x)+square(y)))
#define sign(x)         (((float)(x))==0.0f ? 0.0f : (((float)(x)) > 0.0f ? 1.0f : -1.0f))

#endif // COMMON_UTIL_CONST_H

