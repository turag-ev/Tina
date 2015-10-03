#define TURAG_DEBUG_LOG_SOURCE "_"

#include "image.h"
#include "print.h"
#include <tina/utils/base64.h>


#if TURAG_DEBUG_ENABLE_IMAGE

static unsigned turag_image_index = 0;

/**
 * Fun Fact: Typ und Meldungslevel ergeben zusammen :B
 *
 * Nachrichten im Protokoll (not exactly EBNF, but for consistency with the rest of tina):
 * \verbatim
<Kanalname, [%s]><' '>
<Timestamp in Systemticks, [%d]><' '>
<Farbraum, [R]=RGB><' '>
<x-Auflösung in Pixel, [%d]><' '>
<y-Auflösung in Pixel, [%d]><' '>
<Farbtiefe, Bytes (!=Bits) pro Pixel [%d]><' '>
<Bild-Daten, Base64-kodiert>
\endverbatim
 */

unsigned turag_image_transmit(const char* channel_name, const unsigned timestamp_ticks, const char colorspace_abbrev,
                              const unsigned resolution_x, const unsigned resolution_y, const unsigned resolution_depth,
                              const char* image_buffer)
{
    // we catch these as they can result in endless dumps
    if (resolution_x > 7680 || resolution_y > 4320) {
        turag_error("turag_image_transmit user should buy the extension package to support resolutions above UltraHD");
        return 0;
    }
    if (resolution_depth > 4) {
        turag_error("turag_image_transmit user should buy the extension package to support pixel depths above 32 bit");
        return 0;
    }

    // print all values except image
    turag_debug_printf(TURAG_DEBUG_LINE_PREFIX TURAG_DEBUG_IMAGE_TRANSMISSION TURAG_DEBUG_IMAGE_PREFIX "%s %u %c %u %u %u ",
                       channel_name, timestamp_ticks, colorspace_abbrev, resolution_x, resolution_y, resolution_depth);

    // print base64-encoded image data
    uint8_t encoded[7] = {0};
    unsigned i;
	const unsigned buffer_pixels = (resolution_x * resolution_y * resolution_depth)/4;
    for (i = 0; i < buffer_pixels; ++i) {
        turag_base64_encode((const uint8_t*)&image_buffer[i*4], 4, encoded);
        turag_debug_puts((char*)encoded);
    }

    turag_debug_puts(TURAG_DEBUG_NEWLINE);

    return turag_image_index++;
}

void turag_image_doge(void)
{
#ifdef TURAG_DEBUG_ENABLE_IMAGE_DOGE
    const char doge[] = {
        0xa,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,
        0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x84,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,
        0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x84,
        0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,0x91,
        0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,
        0x96,0x8c,0xe2,0x96,0x92,0xe2,0x96,0x88,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,
        0xe2,0x96,0x91,0xe2,0x96,0x84,0xe2,0x96,0x80,0xe2,0x96,0x92,0xe2,0x96,0x8c,0xe2,0x96,0x91,0xe2,
        0x96,0x91,0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,
        0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x8c,0xe2,0x96,0x92,0xe2,0x96,
        0x92,0xe2,0x96,0x88,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,
        0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x84,0xe2,0x96,0x80,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x90,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xa,0xe2,
        0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xe2,0x96,0x90,0xe2,0x96,0x84,0xe2,0x96,0x80,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x80,
        0xe2,0x96,0x80,0xe2,0x96,0x80,0xe2,0x96,0x80,0xe2,0x96,0x84,0xe2,0x96,0x84,0xe2,0x96,0x84,0xe2,
        0x96,0x80,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,
        0x90,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x84,0xe2,0x96,0x84,0xe2,0x96,0x80,0xe2,0x96,0x92,
        0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x88,0xe2,0x96,0x92,0xe2,0x96,
        0x92,0xe2,0x96,0x84,0xe2,0x96,0x88,0xe2,0x96,0x92,0xe2,0x96,0x90,0xe2,0x96,0x91,0xe2,0x96,0x91,
        0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x84,0xe2,0x96,0x80,
        0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,
        0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,
        0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x80,0xe2,0x96,0x88,0xe2,0x96,0x88,0xe2,0x96,0x80,
        0xe2,0x96,0x92,0xe2,0x96,0x8c,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,
        0xe2,0x96,0x91,0xe2,0x96,0x90,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x84,0xe2,
        0x96,0x84,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,
        0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x80,0xe2,0x96,0x84,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,
        0x96,0x8c,0xe2,0x96,0x91,0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x8c,0xe2,
        0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x8c,0xe2,0x96,0x88,0xe2,0x96,0x80,0xe2,0x96,0x92,0xe2,0x96,
        0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x84,0xe2,0x96,0x80,0xe2,0x96,0x88,
        0xe2,0x96,0x84,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x88,0xe2,0x96,0x92,0xe2,0x96,0x90,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,0x90,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,
        0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,
        0xe2,0x96,0x92,0xe2,0x96,0x8c,0xe2,0x96,0x88,0xe2,0x96,0x88,0xe2,0x96,0x80,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,
        0x92,0xe2,0x96,0x80,0xe2,0x96,0x84,0xe2,0x96,0x8c,0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,
        0x8c,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x84,0xe2,0x96,0x88,0xe2,0x96,0x88,0xe2,0x96,0x84,
        0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,
        0xe2,0x96,0x8c,0xe2,0x96,0x91,0xa,0xe2,0x96,0x80,0xe2,0x96,0x92,0xe2,0x96,0x80,0xe2,0x96,0x90,
        0xe2,0x96,0x84,0xe2,0x96,0x88,0xe2,0x96,0x84,0xe2,0x96,0x88,0xe2,0x96,0x8c,0xe2,0x96,0x84,0xe2,
        0x96,0x91,0xe2,0x96,0x80,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,
        0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x90,0xe2,0x96,0x91,0xa,
        0xe2,0x96,0x90,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x90,0xe2,0x96,0x80,0xe2,0x96,0x90,0xe2,
        0x96,0x80,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x84,0xe2,0x96,0x84,0xe2,0x96,0x92,0xe2,0x96,
        0x84,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,
        0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x8c,0xa,0xe2,0x96,0x90,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x80,0xe2,0x96,0x80,0xe2,0x96,0x84,0xe2,0x96,0x84,0xe2,0x96,
        0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x84,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,
        0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,
        0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,
        0x90,0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,0x8c,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,
        0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x80,0xe2,0x96,0x80,0xe2,0x96,0x80,
        0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,
        0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,
        0x91,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x8c,0xe2,0x96,0x91,0xa,0xe2,0x96,
        0x91,0xe2,0x96,0x90,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,
        0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,
        0x92,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x84,0xe2,0x96,0x92,0xe2,0x96,0x92,
        0xe2,0x96,0x90,0xe2,0x96,0x91,0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x80,
        0xe2,0x96,0x84,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,
        0x91,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x91,0xe2,0x96,0x92,0xe2,0x96,0x84,
        0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x8c,0xe2,0x96,0x91,0xe2,
        0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x80,0xe2,
        0x96,0x84,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,
        0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x84,0xe2,0x96,0x84,
        0xe2,0x96,0x84,0xe2,0x96,0x80,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,
        0x96,0x84,0xe2,0x96,0x80,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,
        0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x80,0xe2,0x96,
        0x84,0xe2,0x96,0x84,0xe2,0x96,0x84,0xe2,0x96,0x84,0xe2,0x96,0x84,0xe2,0x96,0x84,0xe2,0x96,0x80,
        0xe2,0x96,0x80,0xe2,0x96,0x80,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x84,0xe2,0x96,0x84,0xe2,0x96,0x80,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xa,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x92,
        0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,
        0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x92,0xe2,0x96,0x80,0xe2,0x96,0x80,0xe2,0x96,0x91,0xe2,0x96,
        0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,0xe2,0x96,0x91,
        0xa
    };

    turag_debug_puts(doge);
#endif // TURAG_DEBUG_ENABLE_IMAGE_DOGE
}

#endif // TURAG_DEBUG_ENABLE_IMAGE

