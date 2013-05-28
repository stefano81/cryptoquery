#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/aes.h>
#include "schemas/utils.h"

#include "schemas/hve_ip.h"

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

void test_fixed2(const char *path) {
  pairing_t * pairing = load_pairing(path);

  int x[] = {1, 2, 3, 4, 5};
  int y[] = {1, 2, 3, 4, 5};

  setup_t out = setup(pairing, 5);

  mpk_t public = out->public;
  msk_t private = out->private;

  element_t et;
  element_init_GT(et, *pairing);

  element_printf("gT: %B\n", public->gT);

  for (int t = 0; t <= public->l; ++t) {
    element_t ** B = public->B[t];
    element_t ** C = private->C[t];

    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
	element_prod_pairing(et, B[i], C[j], 3);
	element_printf("%d %d %d %B\n", t, i, j, et);
      }
    }
  }

  printf("\n\n\n TESTING ct\n\n\n");

  int ixt = 5;

  element_t w0, wt, dt, st, xt, v[3], *ct, *kt;

  element_init_Zr(xt, *pairing);
  element_init_Zr(w0, *pairing);
  element_init_Zr(wt, *pairing);
  element_init_Zr(dt, *pairing);
  element_init_Zr(st, *pairing);

  element_init_Zr(v[0], *pairing);
  element_init_Zr(v[1], *pairing);
  element_init_Zr(v[2], *pairing);

  for (int t = 1; t <= public->l; ++t) {
    element_random(dt);
    element_random(st);
    element_random(w0);
    element_random(wt);

    element_set_si(xt, ixt);

    element_mul(v[0], dt, xt);
    element_neg(v[1], dt);
    element_set(v[2], st);

    kt = vector_times_matrix(pairing, v, private->C[t], 3);   // kt = (dt * yt, −dt, st)_{Ct} -> set yt = xt;

    element_set(v[0], wt);
    element_mul(v[1], wt, xt);
    element_set(v[2], w0);

    ct = vector_times_matrix(pairing, v, public->B[t], 3);// ct = (wt, wt * xt, w0)_{Bt}

    element_t t1, t2, t3;
    element_init_Zr(t1, *pairing);
    element_init_Zr(t2, *pairing);
    element_init_Zr(t3, *pairing);

    element_sub(t2, xt, xt);
    element_mul(t3, w0, st);
    element_mul(t1, dt, wt);
    element_mul(t1, t1, t2);
    element_add(t1, t1, t3);

    element_pow_zn(et, public->gT, t1);

    element_t p;
    element_init_GT(p, *pairing);

    //element_printf("gT^{*}: %B\n", et);

    element_prod_pairing(p, kt, ct, 3);

    printf("[%d] gT^{*} == e(kt, ct): %s\n", t, 0 == element_cmp(et, p) ? "true":"false");

    //element_printf("e(kt, ct): %B\n", et);
  }

  //e(kt, ct) = g_T^{dt * wt * (xt - yt) + w0 * st}

  element_t m, *dm;

  //  ciphertext_t ct = encrypt(pairing, out->public, x, &m);

  //dkey_t key = keygen(pairing, out->private, y);

  

  //  dm = decrypt(pairing, ct, key1);
  //int r = element_cmp(m, *dm);

}

int main(int argc, char ** argv) {
  if (2 == argc)
    test_fixed2(argv[1]);
    //test_fixed(argv[1]);
  /*   else if (4 == argc) */
  /*     test_variable(argv[1], atoi(argv[2]), atoi(argv[3])); */
  else
    printf("error testing");

  return 0;
}
