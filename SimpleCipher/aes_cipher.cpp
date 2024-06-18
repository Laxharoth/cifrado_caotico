#include <math.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pgm_image.cpp"
#include "pgm_image.h"

#define SAMPLE_SIZE 1000

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *aad,
            int aad_len, unsigned char *tag, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *aad,
            int aad_len, unsigned char *key, unsigned char *iv,
            unsigned char *ciphertext, unsigned char *tag);

void handleErrors(void);

int main(int argc, char *argv[]) {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    /* Set up the key and iv. Do I need to say to not hard code these in a real
     * application? :-) */

    /* A 256 bit key */
    static const unsigned char key[] = "01234567890123456789012345678901";

    /* A 128 bit IV */
    static const unsigned char iv[] = "0123456789012345";

    if (argc < 3) {
        printf("requires \"input file\", \"output file\"");
        exit(EXIT_FAILURE);
    }
    printf(argv[1]);
    printf(argv[2]);
    const std::string input_file = argv[1];
    const std::string output_file = argv[2];

    /* Some additional data to be authenticated */
    static const unsigned char aad[] = "Some AAD data";

    // read image to cipher
    struct PGMImage img = read_pgm(input_file);
    uint64_t size_to_padded_buffer =
        (uint64_t)ceil(((double)img.pixels.size()) / sizeof(uint64_t));

    // pad image data to cipher
    std::unique_ptr<uint64_t> padded_buffer(
        new uint64_t[size_to_padded_buffer]);
    memcpy(padded_buffer.get(), img.pixels.data(), img.pixels.size());

    /* Buffer for the tag */
    unsigned char tag[16];

    int decryptedtext_len = 0, ciphertext_len = 0;

    /* Encrypt the plaintext */
    ciphertext_len =
        encrypt((uint8_t *)padded_buffer.get(),
                size_to_padded_buffer * sizeof(uint64_t), (unsigned char *)aad,
                strlen((char *)aad), (unsigned char *)key, (unsigned char *)iv,
                (uint8_t *)padded_buffer.get(), tag);

    // return pixels to image
    memcpy(img.pixels.data(), padded_buffer.get(), img.pixels.size());

    write_pgm(output_file, img);
    /* Remove error strings */
    ERR_free_strings();

    return 0;
}

void handleErrors(void) {
    unsigned long errCode;

    printf("An error occurred\n");
    while ((errCode = ERR_get_error())) {
        char *err = ERR_error_string(errCode, NULL);
        printf("%s\n", err);
    }
    abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *aad,
            int aad_len, unsigned char *key, unsigned char *iv,
            unsigned char *ciphertext, unsigned char *tag) {
    EVP_CIPHER_CTX *ctx = NULL;
    int len = 0, ciphertext_len = 0;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the encryption operation. */
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
        handleErrors();

    /* Set IV length if default 12 bytes (96 bits) is not appropriate */
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL))
        handleErrors();

    /* Initialise key and IV */
    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv)) handleErrors();

    /* Provide any AAD data. This can be called zero or more times as
     * required
     */
    if (aad && aad_len > 0) {
        if (1 != EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len))
            handleErrors();
    }

    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (plaintext) {
        if (1 !=
            EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
            handleErrors();

        ciphertext_len = len;
    }

    /* Finalise the encryption. Normally ciphertext bytes may be written at
     * this stage, but this does not occur in GCM mode
     */
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    /* Get the tag */
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
        handleErrors();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *aad,
            int aad_len, unsigned char *tag, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx = NULL;
    int len = 0, plaintext_len = 0, ret;

    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the decryption operation. */
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
        handleErrors();

    /* Set IV length. Not necessary if this is 12 bytes (96 bits) */
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL))
        handleErrors();

    /* Initialise key and IV */
    if (!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) handleErrors();

    /* Provide any AAD data. This can be called zero or more times as
     * required
     */
    if (aad && aad_len > 0) {
        if (!EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len)) handleErrors();
    }

    /* Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if (ciphertext) {
        if (!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext,
                               ciphertext_len))
            handleErrors();

        plaintext_len = len;
    }

    /* Set expected tag value. Works in OpenSSL 1.0.1d and later */
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
        handleErrors();

    /* Finalise the decryption. A positive return value indicates success,
     * anything else is a failure - the plaintext is not trustworthy.
     */
    ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    if (ret > 0) {
        /* Success */
        plaintext_len += len;
        return plaintext_len;
    } else {
        /* Verify failed */
        return -1;
    }
}