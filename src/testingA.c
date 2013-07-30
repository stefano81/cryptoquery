#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>

#include <openssl/aes.h>
#include "schemas/utils.h"

#include "schemas/hve_ip_amortized.h"

void test_fixed(const char *path) {
  pairing_t * pairing = load_pairing(path);

  printf("test fixed\n");

  /*  int x[] = {1, 2, 3, 4, 5};
  int x1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int y1[] = {1, 2, 3, 4, 5, 1};
  int y2[] = {-1, -1, 3, 4, 5, 2};
  int y3[] = {2, 3, 4, 5, 6, 9};*/

  int x[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int x1[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  int y1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1};

  int n = 10;
  int l = 10;

  setup_t* out = setup_amortized(pairing, l);
  element_t *m, *dm;

  m = malloc(sizeof(element_t) * 10);

  ciphertext_t* ct = encrypt_amortized(pairing, out->public, x, n, x1, m);

  fprintf(stderr, "test_fixed: ct->n = %d\n", ct->n);

  dkey_t* key1 = keygen_amortized(pairing, out->private, y1);
  dm = decrypt_amortized(pairing, ct, key1);
  for (int i = 0; i < n; ++i) {
    int r = element_cmp(m[i], dm[i]);
    //printf("1: %d %s\n", i, !r ? "OK!" : "No!");
  }
}


void test_EandD(const char *path, int l, int n) {
  fprintf(stderr, "test_EandD %d %d\n", l, n);

  struct timeval tvb, tve;

  element_t *m = malloc(sizeof(element_t) * n);
  int *X,*Y, *X1;
  pairing_t * pairing = load_pairing(path);

  Y=malloc((l + 1)*sizeof(int));
  X=malloc(l*sizeof(int));
  X1 = malloc(sizeof(int) * n);

  for (int i=0;i<l;i++){
    Y[i]=i+1;
    X[i]=i+1;
  }
  Y[l] = 1;

  for (int i = 0; i < n; ++i)
    X1[i] = 1;

  gettimeofday(&tvb, NULL);
  setup_t* out = setup_amortized(pairing,l);
  gettimeofday(&tve, NULL);

  printf("%d setup %lu\n", l, ((tve.tv_sec + (1000*1000 * tve.tv_usec)) - (tvb.tv_sec + (1000*1000 * tvb.tv_usec))));

  gettimeofday(&tvb, NULL);

  ciphertext_t* ct=encrypt_amortized(pairing, out->public, X, n, X1, m);
  gettimeofday(&tve, NULL);

  printf("%d encrypt %lu\n", l, ((tve.tv_sec + (1000*1000 * tve.tv_usec)) - (tvb.tv_sec + (1000*1000 * tvb.tv_usec))));

  gettimeofday(&tvb, NULL);
  dkey_t* tok=keygen_amortized(pairing, out->private,Y);
  gettimeofday(&tve, NULL);

  printf("%d keygen %lu\n", l, ((tve.tv_sec + (1000*1000 * tve.tv_usec)) - (tvb.tv_sec + (1000*1000 * tvb.tv_usec))));

  for (int j = 0; j < 10; ++j) {
    gettimeofday(&tvb, NULL);
    element_t*dm=decrypt_amortized(pairing,ct,tok);
    gettimeofday(&tve, NULL);

    printf("%d decrypt %lu\n", l, ((tve.tv_sec + (1000*1000 * tve.tv_usec)) - (tvb.tv_sec + (1000*1000 * tvb.tv_usec))));

    for (int i = 0; i < n; ++i) {
      int r = element_cmp(m[i], dm[i]);
      fprintf(stderr, "%d %d - %d: %s\n", j, i, r, !r ? "OK!" : "No!");
    }
  }
}

int main(int argc, char ** argv) {
  //printf("%s %s %s\n", argv[0], argv[1], argv[2]);
  if (2 == argc)
    test_fixed(argv[1]);
  else if (3 == argc)
    test_EandD(argv[1], atoi(argv[2]), 1);
  else if (4 == argc)
    test_EandD(argv[1], atoi(argv[2]), atoi(argv[3]));

  return 0;
}
