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
  element_t sigma;
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

lostw_setup_param * lostw_setup(unsigned int n);
lostw_key * lostw_keygen(lostw_general_params * params, lostw_msk * secret, element_t *y);


#endif
