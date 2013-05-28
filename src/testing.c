#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/aes.h>
#include "schemas/utils.h"

#include "schemas/hve_ip.h"

void minorX(element_t * tmp1, element_t a, element_t b, element_t c, element_t d){
  element_t tmp2;

  element_init_same_as(*tmp1,a);
  element_init_same_as(tmp2,a);
  element_mul(*tmp1,a,b);
  element_mul(tmp2,c,d);
  element_sub(*tmp1,*tmp1,tmp2);

  element_clear(tmp2);
}
    
void test_fixed(const char *path) {
  pairing_t * pairing = load_pairing(path);

  int x[] = {1, 2, 3, 4, 5};
  int y1[] = {1, 2, 3, 4, 5};
  int y2[] = {-1, -1, 3, 4, 5};
  int y3[] = {2, 3, 4, 5, 6};

  setup_t out = setup(pairing, 5);
  //setup_t out = setupAlgo(pairing, 5);
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
  printf("3: %s\n", !r ? "OK!" : "No!");
}

int main(int argc, char ** argv) {
  if (2 == argc)
    test_fixed(argv[1]);
  /*   else if (4 == argc) */
  /*     test_variable(argv[1], atoi(argv[2]), atoi(argv[3])); */
  else
    printf("error testing");

  return 0;
}
