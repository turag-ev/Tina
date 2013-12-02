/**
 *  @brief		Base64 encoding/decoding routines
 *  @file		base64.h
 *  @date		02.12.2013
 *  @author		Martin Oemus <martin@oemus.net>
 * 
 */
#ifndef BASE64_H_
#define BASE64_H_


#ifdef __cplusplus
extern "C" {
#endif



/**
 * Calculates length of decoded string.
 * @param[in] bufcoded base64 encoded string
 * @return length of decoded string
 */
int turag_base64_decode_len(const char *bufcoded);

/**
 * Decodes base64 encoded string
 * @param[in] bufcoded base64 encoded string
 * @param[out] bufplain buffer holding decoded data
 * @return Length of decoded data
 */
int turag_base64_decode(const char *bufcoded, char *bufplain);


/**
 * Calculates length of base64 encoded string
 * @param[in] len Length data that is to be encoded
 * @return length of base64 encoded string
 */
int turag_base64_encode_len(int len);

/**
 * Encodes data to base64 encoded string
 * @param[in] string data to encode
 * @param[in] len length of data
 * @param[out] encoded buffer holding base64 encoded string
 * @return
 */
int turag_base64_encode(const char *string, int len, char *encoded);



#ifdef __cplusplus
} // extern "C"
#endif


#endif /* BASE64_H_ */
