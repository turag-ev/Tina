#ifndef TINA_DEBUG_IMAGE_H
#define TINA_DEBUG_IMAGE_H

#include <tina/debugprint.h>
#include "defines.h"




/// \addtogroup Debug
/// \{
/// \defgroup Debugimage Bilddaten-Ausgabe
/// \brief Ausgabe von Bildern/Video-Streams.
/// \{

// output images
#if !defined(TURAG_DEBUG_ENABLE_IMAGE)
# if TURAG_DEBUG_LEVEL > 3 || defined(__DOXYGEN__)
/// \brief Wenn definiert werden Bilder ausgegeben
///
/// Kann über Makefile oder in Zeilen vor erstem Include definiert werden.
/// Wird standardmäßig bei TURAG_DEBUG_LEVEL größer 3 definiert.
#  define TURAG_DEBUG_ENABLE_IMAGE 1
# else
#  define TURAG_DEBUG_ENABLE_IMAGE 0
# endif
#endif

#ifndef __DOXYGEN__

// graph output prefixes
#define TURAG_DEBUG_IMAGE_PREFIX          "B"
#define TURAG_DEBUG_IMAGE_TRANSMISSION    ":"

#endif // __DOXYGEN__


#if TURAG_DEBUG_ENABLE_IMAGE || defined(__DOXYGEN__)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Transmits an image.
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

/**
 * @brief Do nothing, so scare. Or maybe print something if TURAG_DEBUG_ENABLE_IMAGE_DOGE is set.
 * @verbatim
░░░░░░░░░▄░░░░░░░░░░░░░░▄░░░░
░░░░░░░░▌▒█░░░░░░░░░░░▄▀▒▌░░░
░░░░░░░░▌▒▒█░░░░░░░░▄▀▒▒▒▐░░░
░░░░░░░▐▄▀▒▒▀▀▀▀▄▄▄▀▒▒▒▒▒▐░░░
░░░░░▄▄▀▒░▒▒▒▒▒▒▒▒▒█▒▒▄█▒▐░░░
░░░▄▀▒▒▒░░░▒▒▒░░░▒▒▒▀██▀▒▌░░░
░░▐▒▒▒▄▄▒▒▒▒░░░▒▒▒▒▒▒▒▀▄▒▒▌░░
░░▌░░▌█▀▒▒▒▒▒▄▀█▄▒▒▒▒▒▒▒█▒▐░░
░▐░░░▒▒▒▒▒▒▒▒▌██▀▒▒░░░▒▒▒▀▄▌░
░▌░▒▄██▄▒▒▒▒▒▒▒▒▒░░░░░░▒▒▒▒▌░
▀▒▀▐▄█▄█▌▄░▀▒▒░░░░░░░░░░▒▒▒▐░
▐▒▒▐▀▐▀▒░▄▄▒▄▒▒▒▒▒▒░▒░▒░▒▒▒▒▌
▐▒▒▒▀▀▄▄▒▒▒▄▒▒▒▒▒▒▒▒░▒░▒░▒▒▐░
░▌▒▒▒▒▒▒▀▀▀▒▒▒▒▒▒░▒░▒░▒░▒▒▒▌░
░▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒░▒░▒░▒▒▄▒▒▐░░
░░▀▄▒▒▒▒▒▒▒▒▒▒▒░▒░▒░▒▄▒▒▒▒▌░░
░░░░▀▄▒▒▒▒▒▒▒▒▒▒▄▄▄▀▒▒▒▒▄▀░░░
░░░░░░▀▄▄▄▄▄▄▀▀▀▒▒▒▒▒▄▄▀░░░░░
░░░░░░░░░▒▒▒▒▒▒▒▒▒▒▀▀░░░░░░░░
@endverbatim
 */
void turag_image_doge(void);

#else // TURAG_DEBUG_ENABLE_IMAGE

# define turag_image_transmit(channel_name, timestamp_ticks, colorspace_abbrev, resolution_x, resolution_y, resolution_depth, image_buffer) TURAG_MACRO_NOOP

#endif // TURAG_DEBUG_ENABLE_IMAGE

#ifdef __cplusplus
}
#endif

/// \}
/// \}
/// \}

#endif // TINA_DEBUG_IMAGE_H
