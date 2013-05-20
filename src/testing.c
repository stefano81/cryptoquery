#include <stdio.h>

#include <openssl/aes.h>

int main(int argc, char ** argv) {
  const unsigned char * passwd = "this is the key";
  const unsigned char * plaintext = "This is a simple plaintext\nLet's try if it works!!";
  AES_KEY key;
  unsigned char ciphertext[4 * 1024 * 1024];
  unsigned char pt2[4 * 1024 * 1024];


  printf("set password: %d\n", AES_set_encrypt_key(passwd, 128, &key));

  AES_encrypt(plaintext, ciphertext, &key);
  printf("plaintext:\n'''\n%s\n'''\n", plaintext);
  //  printf("ciphertext:\n'''\n%s\n'''\n", ciphertext);

  //  AES_decrypt(ciphertext, pt2, key);

  printf("decrypttext:\n'''\n%s\n'''\n", pt2);

  return 0;
}
