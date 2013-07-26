#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>

#include <openssl/aes.h>
#include "schemas/utils.h"

#include "schemas/hve_ip_amortized.h"

void test_fixed(const char *path) {
  pairing_t * pairing = load_pairing(path);

  int x[] = {1, 2, 3, 4, 5};
  int x1[] = {1, 2};
  int y1[] = {1, 2, 3, 4, 5, 1};
  int y2[] = {-1, -1, 3, 4, 5, 2};
  int y3[] = {2, 3, 4, 5, 6, 9};

  setup_t* out = setup_amortized(pairing, 5);
  element_t m[2], *dm;

  ciphertext_t* ct = encrypt_amortized(pairing, out->public, x, 2, x1, m);

  dkey_t* key1 = keygen_amortized(pairing, out->private, y1);
  dm = decrypt_amortized(pairing, ct, key1);
  //int r = element_cmp(m, *dm);
  //printf("1: %s\n", !r ? "OK!" : "No!");

  dkey_t* key2 = keygen_amortized(pairing, out->private, y2);
  dm = decrypt_amortized(pairing, ct, key2);
  //  r = element_cmp(m, *dm);
  //  printf("2: %s\n", !r ? "OK!" : "No!");

  dkey_t* key3 = keygen_amortized(pairing, out->private, y3);
  dm = decrypt_amortized(pairing, ct, key3);
  //  r = element_cmp(m, *dm);
  //  printf("3: %s\n", !r ? "OK!" : "No!");
}

/* void test_fixed2(const char *path) { */
/*   pairing_t * pairing = load_pairing(path); */

/*   int x[] = {1, 2, 3, 4, 5}; */
/*   int y[] = {1, 2, 3, 4, 5}; */

/*   setup_t* out = setup_amortized(pairing, 5); */

/*   mpk_t* public = out->public; */
/*   msk_t* private = out->private; */

/*   element_t et; */
/*   element_init_GT(et, *pairing); */

/*   element_printf("gT: %B\n", public->gT); */

/*   for (int t = 0; t <= public->l; ++t) { */
/*     element_t ** B = public->B[t];
/*     element_t ** C = private->C[t]; 
/*     for (int i = 0; i < 3; ++i) { 
/*       for (int j = 0; j < 3; ++j) { 
/* 	element_prod_pairing(et, B[i], C[j], 3); 
/* 	element_printf("%d %d %d %B\n", t, i, j, et); 
/*       } 
/*     } 
/*   } 

/*   printf("\n\n\n TESTING ct\n\n\n"); 

/*   int ixt = 5; 

/*   element_t w0, wt, dt, st, xt, v[3], *ct, *kt, s0, z, eta; 
/*   element_t prod; 
/*   element_t m; 

/*   element_t p; 
/*   element_t t1, t2, t3; 

/*   element_init_GT(prod, *pairing); 
/*   element_init_GT(m, *pairing); 

/*   element_set1(prod); 
/*   element_random(m); 

/*   element_init_Zr(t1, *pairing); 
/*   element_init_Zr(t2, *pairing); 
/*   element_init_Zr(t3, *pairing); 

/*   element_init_GT(p, *pairing); 


/*   element_init_Zr(eta, *pairing); 
/*   element_init_Zr(z, *pairing); 
/*   element_init_Zr(s0, *pairing); 
/*   element_init_Zr(xt, *pairing); 
/*   element_init_Zr(w0, *pairing); 
/*   element_init_Zr(wt, *pairing); 
/*   element_init_Zr(dt, *pairing); 
/*   element_init_Zr(st, *pairing); 

/*   element_init_Zr(v[0], *pairing); 
/*   element_init_Zr(v[1], *pairing); 
/*   element_init_Zr(v[2], *pairing); 

/*   element_set0(s0); 
/*   element_random(w0);
/*   element_random(z); 
/*   element_random(eta); 

/*   for (int t = 1; t <= public->l; ++t) { 
/*     element_random(dt); 
/*     element_random(st); 
/*     element_random(wt); 

/*     element_add(s0, s0, st); 

/*     element_set_si(xt, ixt); 

/*     element_mul(v[0], dt, xt); 
/*     element_neg(v[1], dt); 
/*     element_set(v[2], st); 

/*     kt = vector_times_matrix(pairing, v, private->C[t], 3);   // kt = (dt * yt, −dt, st)_{Ct} -> set yt = xt; 

/*     element_set(v[0], wt); 
/*     element_mul(v[1], wt, xt); 
/*     element_set(v[2], w0); 

/*     ct = vector_times_matrix(pairing, v, public->B[t], 3);// ct = (wt, wt * xt, w0)_{Bt} 


/*     element_sub(t2, xt, xt); 
/*     element_mul(t3, w0, st); 
/*     element_mul(t1, dt, wt); 
/*     element_mul(t1, t1, t2); 
/*     element_add(t1, t1, t3); 

/*     element_pow_zn(et, public->gT, t1); 

/*     //element_printf("gT^{*}: %B\n", et); 

/*     element_prod_pairing(p, kt, ct, 3); 
/*     //e(kt, ct) = g_T^{dt * wt * (xt - yt) + w0 * st} 

/*     printf("[%d] gT^{*} == e(kt, ct): %s\n", t, 0 == element_cmp(et, p) ? "true":"false"); 

/*     element_mul(prod, prod, p); 

/*     //element_printf("e(kt, ct): %B\n", et); 
/*   } 

/*   // k0 = (s0, 1, eta)_{B0} 
/*   element_neg(s0, s0); 
/*   element_set(v[0], s0); 
/*   element_set1(v[1]); 
/*   element_set(v[2], eta); 

/*   kt = vector_times_matrix(pairing, v, private->C[0], 3); 

/*   element_set(v[0], w0); 
/*   element_set(v[1], z); 
/*   element_set0(v[2]); 

/*   // c0 = (w0, z, 0)_{C0}; 
/*   ct = vector_times_matrix(pairing, v, public->B[0], 3); 

/*   // e(k0, c0) = g_T^{s0, w0} 

/*   element_mul(t1, s0, w0); 
/*   element_add(t1, t1, z); 
/*   element_pow_zn(et, public->gT, t1); 

/*   element_prod_pairing(p, kt, ct, 3); 


/*   printf("[0] gT^{s0*w0 + z} == e(k0, c0): %s\n", 0 == element_cmp(et, p) ? "true":"false"); 

/*   element_mul(prod, prod, p); 

/*   element_pow_zn(p, public->gT, z); 

/*   printf("gT^z == e(k0, c0) * \\Pi e(kt, ct): %s\n", 0 == element_cmp(p, prod) ? "true":"false"); 

/*   element_t c; 

/*   element_init_GT(c, *pairing); 


   element_mul(c, p, m); // c = gT^z * m 

   element_div(et, c, prod); 

   printf("m == m': %s\n", 0 == element_cmp(m, et) ? "true":"false"); 
  
 } */

void test_EandD(const char *path, int l, int n) {
  fprintf(stderr, "testing EandD %d %d\n", l, n);

  struct timeval tvb, tve;

  element_t *m = malloc(sizeof(element_t) * n);
  int *X,*Y, *X1;
  pairing_t * pairing = load_pairing(path);

  Y=malloc(l*sizeof(int) + 1);
  X=malloc(l*sizeof(int));
  X1 = malloc(sizeof(int) * n);

  for (int i=0;i<l;i++){
    Y[i]=i+1; X[i]=i+1;
  }

  for (int i = 0; i < n; ++i)
    X1[i] = i;

  gettimeofday(&tvb, NULL);
  setup_t* out=setup_amortized(pairing,l);
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

  gettimeofday(&tvb, NULL);
  element_t*dm=decrypt_amortized(pairing,ct,tok);
  gettimeofday(&tve, NULL);

  printf("%d decrypt %lu\n", l, ((tve.tv_sec + (1000*1000 * tve.tv_usec)) - (tvb.tv_sec + (1000*1000 * tvb.tv_usec))));

  for (int i = 0; i < n; ++i) {
    int r = element_cmp(m[i], dm[i]);
    fprintf(stderr, "%d - %d: %s\n", i, r, !r ? "OK!" : "No!");
  }
}

int main(int argc, char ** argv) {
  /*  if (2 == argc)
      test_fixed(argv[1]);
      else if (4 == argc)
      test_variable(argv[1], atoi(argv[2]), atoi(argv[3]));
      else*/
  //  if (3 == argc) {
    test_fixed(argv[1]);
    //  } else if (4 == argc)
    //    test_EandD(argv[1], atoi(argv[2]), atoi(argv[3]));
    //  else
    //    printf("error testing");

  return 0;
}
