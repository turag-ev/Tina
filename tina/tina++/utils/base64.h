#ifndef TINAPP_UTILS_BASE64_H
#define TINAPP_UTILS_BASE64_H


#include <tina/utils/base64.h>
#include <tina++/tina.h>


namespace TURAG {
namespace Base64 {

// These functions do not comply with any RFC as it does not
// produce any trailing `=' padding characters!!!

/**
 * Calculates length of base64 encoded data
 * @param[in] len Length of data that is to be encoded
 * @return length of base64 encoded data
 */
_always_inline
size_t encodeLength(size_t len) {
    return turag_base64_encode_len(len);
}

/**
 * Calculates length of base64 encoded data
 * @param[in] data data that is to be encoded
 * @return length of base64 encoded data
 */
template<typename T> _always_inline
constexpr size_t encodeLength(const T& data) {
    return (sizeof(data) * 4 + 2) / 3;
}

/**
 * Encodes data to base64 encoded data
 * @param[in] data data to encode
 * @param[in] len length of data
 * @param[out] encoded buffer holding base64 encoded data
 * @return length of base64 encoded data
 */
_always_inline
int encode(const uint8_t *data, size_t len, uint8_t *encoded) {
    return turag_base64_encode(data, len, encoded);
}

/**
 * Encodes data to base64 encoded data
 * @param[in] data data to encode
 * @param[out] encoded buffer holding base64 encoded data
 * @return length of base64 encoded data
 */
template<typename T> _always_inline
int encode(const T& data, uint8_t *encoded) {
    return turag_base64_encode(reinterpret_cast<const uint8_t*>(&data), sizeof(data), encoded);
}


/**
 * Calculates length of decoded data.
 * @param[in] len length of base64 encoded data
 * @return length of decoded data
 */
_always_inline
size_t decodeLength(size_t len) {
    return turag_base64_decode_len(len);
}

/**
 * Calculates length of decoded data.
 * @param[in] data base64 encoded data
 * @return length of decoded data
 */
template<typename T> _always_inline
size_t decodeLength(const T& data) {
    return sizeof(data) * 3 / 4;
}

/**
 * Decodes base64 encoded data
 * @param[in] bufcoded base64 encoded data
 * @param[in] len length of base64 encoded data
 * @param[out] bufplain buffer holding decoded data
 * @return Length of decoded data
 */
_always_inline
int decode(const uint8_t *encoded, size_t len, uint8_t *data) {
    return turag_base64_decode(encoded, len, data);
}



} // namespace Base64
} // namespace TURAG


#endif // TINAPP_UTILS_BASE64_H
