#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/aes.h>
#include "schemas/utils.h"

#include "schemas/hve_ip.h"

int main(int argc, char ** argv) {
  pairing_t * pairing = load_pairing(argv[1]);

  int x[] = {1, 2, 3, 4, 5};
  int y1[] = {1, 2, 3, 4, 5};
  int y2[] = {-1, -1, 3, 4, 5};
  int y3[] = {2, 3, 4, 5, 6};

  setup_t out = setup(pairing, 5);
  element_t m, *dm;

  ciphertext_t ct = encrypt(pairing, out->public, x, &m);

  dkey_t key1 = keygen(pairing, out->private, y1);
  dm = decrypt(pairing, ct, key1);
  int r = element_cmp(m, *dm);
  printf("1: %s\n", !r ? "OK!" : "No!");

  dkey_t key2 = keygen(pairing, out->private, y2);
  dm = decrypt(pairing, ct, key2);
  r = element_cmp(m, *dm);
  printf("2: %s\n", !r ? "OK!" : "No!");

  dkey_t key3 = keygen(pairing, out->private, y3);
  dm = decrypt(pairing, ct, key3);
  r = element_cmp(m, *dm);
  printf("3: %s\n", r ? "OK!" : "No!");

  return 0;
}
