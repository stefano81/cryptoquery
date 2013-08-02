#include "aes.h"

#include <string.h>

#include <openssl/aes.h>
#include <openssl/sha.h>


static void init_AES(element_t k, int enc, AES_KEY *key, unsigned char *iv) {
  int keylen = element_length_in_bytes(k);

  if (keylen < 17)
    keylen = 17;

  unsigned char *keybuff = malloc(keylen);

  element_to_bytes(keybuff, k);

  if (enc)
    AES_set_encrypt_key(keybuff + 1, 128, key);
  else
    AES_set_decrypt_key(keybuff + 1, 128, key);

  free(keybuff);

  memset(iv, 0, AES_BLOCK_SIZE);
}

unsigned char * encrypt_AES(element_t k, int *val, int size) {
  AES_KEY key;
  unsigned char iv[AES_BLOCK_SIZE];

  int len = ((int) ((sizeof(int) * size) / AES_BLOCK_SIZE)) * AES_BLOCK_SIZE + (AES_BLOCK_SIZE * ((sizeof(int) * size % AES_BLOCK_SIZE) > 0 ? 1 : 0));

  unsigned char *outbuff = malloc(sizeof(unsigned char) * len);

  init_AES(k, 1, &key, iv);

  AES_cbc_encrypt((unsigned char *)val, outbuff, (size * sizeof(int)), &key, iv, AES_ENCRYPT);

  return outbuff;
}

int * decrypt_AES(element_t k, unsigned char *buffer, int size) {
  AES_KEY key;
  unsigned char iv[16];

  init_AES(k, 0, &key, iv);

  int *out = malloc(sizeof(int) * size);


  AES_cbc_encrypt(buffer, (unsigned char *)out, (size * sizeof(int)), &key, iv, AES_DECRYPT);

  return out;
}

