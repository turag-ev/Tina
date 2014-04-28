#ifndef TINA_DEBUG_IMAGE_H
#define TINA_DEBUG_IMAGE_H

#include <tina/debugprint.h>
#include "defines.h"

/// \addtogroup Debug
/// \{
/// \defgroup Debug Ausgabe von Bilddaten
/// \{

// graph output prefixes
#define TURAG_DEBUG_IMAGE_PREFIX          "B"
#define TURAG_DEBUG_IMAGE_TRANSMISSION    ":"

#ifdef TURAG_DEBUG_ENABLE_IMAGE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief turag_image_transmit Transmits an image.
 * @param channel_name Name of image source.
 * @param timestamp_ticks System tick value when image was shot
 * @param colorspace_abbrev Color space: H=HSV, R=RGB
 * @param resolution_x Image width
 * @param resolution_y Image height
 * @param resolution_depth Bytes per pixel
 * @param image_buffer Pointer to image buffer with length x*y*depth
 * @return Index of the newly created image.
 */
unsigned turag_image_transmit(const char* channel_name, const unsigned timestamp_ticks, const char colorspace_abbrev,
                              const unsigned resolution_x, const unsigned resolution_y, const unsigned resolution_depth,
                              const char* image_buffer);

#else // TURAG_DEBUG_ENABLE_IMAGE

# define turag_image_transmit(channel_name, timestamp_ticks, colorspace_abbrev, resolution_x, resolution_y, resolution_depth, image_buffer) while(0)

#endif // TURAG_DEBUG_ENABLE_IMAGE

#ifdef __cplusplus
}
#endif

/// \}
/// \}
/// \}

#endif // TINA_DEBUG_IMAGE_H
