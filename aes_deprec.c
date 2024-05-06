#include <openssl/aes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLE_SIZE 1000
#include "time_measure.h"

#define AES_BLOCK_SIZE 16

void encrypt_data(const unsigned char *input, unsigned char *output,
                  int input_len, const unsigned char *key) {
    AES_KEY aes_key;
    unsigned char iv[AES_BLOCK_SIZE];
    int i;

    // Inicializar el vector de inicialización (IV) con ceros
    memset(iv, 0, AES_BLOCK_SIZE);

    // Inicializar la clave para cifrado
    if (AES_set_encrypt_key(key, 128, &aes_key) < 0) {
        fprintf(stderr, "Error al inicializar la clave\n");
        return;
    }

    // Cifrar los datos en bloques de 16 bytes
    for (i = 0; i < input_len; i += AES_BLOCK_SIZE) {
        AES_encrypt(input + i, output + i, &aes_key);
    }
}

void decrypt_data(const unsigned char *input, unsigned char *output,
                  int input_len, const unsigned char *key) {
    AES_KEY aes_key;
    unsigned char iv[AES_BLOCK_SIZE];
    int i;

    // Inicializar el vector de inicialización (IV) con ceros
    memset(iv, 0, AES_BLOCK_SIZE);

    // Inicializar la clave para cifrado
    if (AES_set_encrypt_key(key, 128, &aes_key) < 0) {
        fprintf(stderr, "Error al inicializar la clave\n");
        return;
    }

    // Cifrar los datos en bloques de 16 bytes
    for (i = 0; i < input_len; i += AES_BLOCK_SIZE) {
        AES_decrypt(input + i, output + i, &aes_key);
    }
}

int main() {
    const unsigned char key[AES_BLOCK_SIZE] = "1234567890123456";
    int input_len = 1048576;
    unsigned char *input = malloc(input_len);

    // Asegurarse de que la longitud de los datos de entrada sea un múltiplo de
    // AES_BLOCK_SIZE
    int padded_input_len =
        (input_len + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE * AES_BLOCK_SIZE;

    // Añadir relleno si es necesario
    unsigned char padded_input[padded_input_len];
    memset(padded_input, 0, padded_input_len);
    memcpy(padded_input, input, input_len);
    unsigned char *encrypted = malloc(padded_input_len);
    unsigned char *decrypted = malloc(padded_input_len);

    double time_encrypt_secs = time_exec(
        encrypt_data(padded_input, encrypted, padded_input_len, key););
    double time_decrypt_secs =
        time_exec(decrypt_data(encrypted, decrypted, padded_input_len, key););

    printf("Velocidad de cifrado: %0.4f Gbps\n",
           input_len * 8 / time_encrypt_secs / 1e9);
    printf("Velocidad de descifrado: %0.4f Gbps\n",
           input_len * 8 / time_decrypt_secs / 1e9);

    return 0;
}
