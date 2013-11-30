
#include <tina/float.h>

// fuck eCos, we make our own float math functions from eCos double functions

long int lroundf(float x) {
  return (x < 0.0f) ? ceilf(x - 0.5f) : floorf(x + 0.5f);
}
