#include <stdio.h>
#include <string.h>

#include <openssl/aes.h>
#include "schemas/utils.h"

#include "schemas/hve_ip.h"

int main(int argc, char ** argv) {
  const unsigned char * passwd = "this is the key";
  const unsigned char * plaintext = "This is a simple plaintext\nLet's try if it works!!";
  AES_KEY ekey, dkey;
  unsigned char ciphertext[4 * 1024];
  unsigned char pt2[4 * 1024];

  //  printf("set password: %d\n", AES_set_encrypt_key(passwd, 128, &ekey));

  //  for (int processed = 0;processed < strlen(plaintext); processed += AES_BLOCK_SIZE)
  //AES_encrypt(plaintext+processed, ciphertext+processed, &ekey);

  //AES_cfb128_encrypt(plaintext, ciphertext, strlen(plaintext), 

/*   printf("plaintext:\n'''\n%s\n'''\n", plaintext);
   printf("ciphertext:\n'''\n%s\n'''\n%lu\n", ciphertext, strlen(ciphertext)); 

   AES_set_decrypt_key(passwd, 128, &dkey);
   AES_decrypt(ciphertext, pt2, &dkey); */

//  printf("decrypttext:\n'''\n%s\n'''\n", pt2);

  pairing_t * pairing = load_pairing(argv[1]);
  
/*   element_t e[3]; */

/*   element_init_G1(e[1], *pairing); */
/*   element_init_G2(e[2], *pairing); */
/*   element_init_GT(e[0], *pairing); */

/*   void (*fs1[2])(element_t) = {element_set0, element_set1}; */
/*   int (*fs2[2])(element_t) = {element_is0, element_is1}; */

/*   for (int i = 0; i < 3; ++i) { */
/*     printf("i: %d\n", i); */
/*     for (int j = 0; j < 2; ++j) { */
/*       printf("j: %d\n", j); */
/*       fs1[j](e[i]); */
/*       for (int k = 0; k < 2; ++k) { */
/* 	printf("%d %d %d -> %d\n", i, j, k, fs2[k](e[i])); */
/*       } */
/*     } */
/*   } */

  int x[] = {1, 2, 3, 4, 5};
  int y1[] = {1, 2, 3, 4, 5};
  int y2[] = {-1, -1, 3, 4, 5};
  int y3[] = {2, 3, 4, 5, 6};

  setup_t out = setup(*pairing, 5);
  element_t m, *dm;

  ciphertext_t ct = encrypt(pairing, out->public, x, &m);

  dkey_t key1 = keygen(pairing, out->private, y1);
  printf("1: %s\n", element_cmp(m, *decrypt(pairing, ct, key1)) ? "OK!" : "No!");

  dkey_t key2 = keygen(pairing, out->private, y2);
  printf("2: %s\n", element_cmp(m, *decrypt(pairing, ct, key2)) ? "OK!" : "No!");

  dkey_t key3 = keygen(pairing, out->private, y3);
  printf("3: %s\n", element_cmp(m, *decrypt(pairing, ct, key3)) ? "OK!" : "No!");

  return 0;
}
