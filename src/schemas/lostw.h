#ifndef LOSTW_H
#define LOSTW_H

#include <pbc/pbc.h>

typedef struct {
  pairing_t pairing;
  unsigned int n;
  element_t g;
} lostw_general_params;

typedef struct {
  element_t ** B;
} lostw_mpk;

typedef struct {
  element_t ** BStar;
} lostw_msk;

typedef struct {
  lostw_general_params * params;
  lostw_mpk * public;
  lostw_msk * secret;
} lostw_setup_param;

typedef struct {
  element_t * kstar;
} lostw_key;

typedef struct {
  element_t *c1;
} lostw_ct;

lostw_setup_param * lostw_setup(unsigned int n, const char * path);
lostw_key * lostw_keygen(lostw_general_params * params, lostw_msk * secret, element_t * y);
lostw_ct * lostw_enc(lostw_general_params * params, lostw_mpk * private, element_t * x, element_t * m);

#endif
