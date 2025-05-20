/*
 * crypt_aes: Encrypt or decrypt the input message using AES 128 CBC
 *
 * inputs: constant pointer to input text,
 *         length of input text,
 *            must be <= 96 to allow for expansion during encryption
 *         pointer to pointer to dynamically allocated output buffer
 *            may be NULL, caller must free,
 *         constant pointer to AES 128 key,
 *         pointer to pointer to initialization vector (IV),
 *            IV memory must be pre-allocated to BLK_LEN
 *            may _not_ be NULL, caller must free,
 *         mode is either DECRYPT or ENCRYPT (see below)
 *
 * outputs: output buffer contains encrypted/decrypted message,
 *          if mode is ENCRYPT then IV will be populated with encryption IV,
 *          if mode is DECRYPT then IV must already be populated before call
 *
 * returns: the length of the output text on success, zero on failure
 *
 */
#ifndef AES_CRYPTO_H
#define AES_CRYPTO_H

extern const int DECRYPT;
extern const int ENCRYPT;
extern const int BLK_LEN;

int crypt_aes(const unsigned char* in_txt, size_t len, unsigned char** out_txt,
              const unsigned char* key, unsigned char** iv, int mode);

#endif /* AES_CRYPTO_H */
