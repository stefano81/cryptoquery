#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/aes.h>
#include "schemas/utils.h"

#include "schemas/hve_ip.h"

int HVE(int x[], int y[], int n) {
  for (int i = 0; i < n; ++i)
    if (-1 != y[i] && y[i] != x[i])
      return 0;

  return 1;
}

void test_fixed(const char *path) {
  pairing_t * pairing = load_pairing(path);

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
  printf("3: %s\n", !r ? "OK!" : "No!");
}

/* void test_variale(consta char *path, int size, int ntry) { */
/*   pairing_t * pairing = load_pairing(path); */
/*   setup_t out = setup(pairing, size); */

/*   element_t m, *dm; */

/*   int *x = malloc(sizeof(int) * size); */
/*   int **y = malloc(sizeof(int) * ntry); */

/*   for (int i = 0; i < size; ++i) { */
/*     x[i] = (int) (rand() / (double) RAND_MAX) * 100; */
/*   } */

/*   ciphertext_t ct = encrypt(pairing, out->public, x, &m); */

/*   dkey_t k = keygen(pairing, out->private, y1); */
/*   dm = decrypt(pairing, ct, key1); */
/*   int r = element_cmp(m, *dm); */
/*   printf("1: %s\n", !r ? "OK!" : "No!"); */

/*   dkey_t key2 = keygen(pairing, out->private, y2); */
/*   dm = decrypt(pairing, ct, key2); */
/*   r = element_cmp(m, *dm); */
/*   printf("2: %s\n", !r ? "OK!" : "No!"); */

/*   dkey_t key3 = keygen(pairing, out->private, y3); */
/*   dm = decrypt(pairing, ct, key3); */
/*   r = element_cmp(m, *dm); */
/*   printf("3: %s\n", r ? "OK!" : "No!"); */

/* } */

int main(int argc, char ** argv) {
  if (2 == argc)
    test_fixed(argv[1]);
/*   else if (4 == argc) */
/*     test_variable(argv[1], atoi(argv[2]), atoi(argv[3])); */
  else
    printf("error testing");

  return 0;
}
