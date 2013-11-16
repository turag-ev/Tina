
#include <math.h>

// fuck eCos, we make our own float math functions from eCos double functions

float sinf(float x) {
  return sin(x);
}

float asinf(float x) {
  return asin(x);
}

float cosf(float x) {
  return cos(x);
}

float atan2f(float y, float x) {
  return atan2(y, x);
}

float sqrtf(float x) {
  return sqrt(x);
}

long int lroundf(float x) {
  return (x < 0.0f) ? ceil(x - 0.5f) : floor(x + 0.5f);
}
