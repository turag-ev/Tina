/**
 *  @brief		Base64 encoding/decoding routines
 *  @file		tina/utils/base64.h
 *  @date		02.12.2013
 *  @author		Martin Oemus <martin@oemus.net>
 *
 */
#ifndef TINA_UTILS_BASE64_H_
#define TINA_UTILS_BASE64_H_

#include <tina/tina.h>


#ifdef __cplusplus
extern "C" {
#endif

// These functions do not comply with any RFC as it does not
// produce any trailing `=' padding characters!!!

/**
 * Calculates length of base64 encoded data
 * @param[in] len Length of data that is to be encoded
 * @return length of base64 encoded data
 */
size_t turag_base64_encode_len(size_t len);

/**
 * Encodes data to base64 encoded data
 * @param[in] data data to encode
 * @param[in] len length of data
 * @param[out] encoded buffer holding base64 encoded data
 * @return length of base64 encoded data
 */
int turag_base64_encode(const uint8_t *data, size_t len, uint8_t *encoded);


/**
 * Calculates length of decoded data.
 * @param[in] len length of base64 encoded data
 * @return length of decoded data
 */
size_t turag_base64_decode_len(size_t len);

/**
 * Decodes base64 encoded data
 * @param[in] bufcoded base64 encoded data
 * @param[in] len length of base64 encoded data
 * @param[out] bufplain buffer holding decoded data
 * @return Length of decoded data
 */
int turag_base64_decode(const uint8_t *encoded, size_t len, uint8_t *data);




#ifdef __cplusplus
} // extern "C"
#endif


#endif /* TINA_UTILS_BASE64_H_ */
