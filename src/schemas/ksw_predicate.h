#ifndef KSW_H
#define KSW_H

#include <pbc.h>

typedef struct {
  element_t gp;
  element_t gr;
  element_t Q;
  element_t P;
  element_t * H1i;
  element_t * H2i;
  unsigned int n;
} ksw_mpk_t;

typedef struct {
  element_t p, q, r, gq, hl;
  element_t * h1i;
  element_t * h2i;
  unsigned int n;
} ksw_msk_t;

typedef struct {
  pairing_t pairing;
  ksw_mpk_t * public;
  ksw_msk_t * secret;
} ksw_setup_t;

typedef struct {
  element_t C;
  element_t C1;
  element_t * C1i;
  element_t * C2i;
  unsigned int n;
} ksw_ct_t;

typedef struct {
  element_t K;
  element_t * K1i;
  element_t * K2i;
  unsigned int n;
} ksw_key_t;

ksw_setup_t * ksw_setup(unsigned int n);
ksw_ct_t * ksw_enc(ksw_mpk_t * public, element_t * x, element_t * m);
ksw_key_t * ksw_genkey(ksw_msk_t * secret, element_t * v);
element_t * ksw_dec(ksw_key_t * key, ksw_ct_t * ct);

#endif
