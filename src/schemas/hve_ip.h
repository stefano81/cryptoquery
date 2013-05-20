#ifndef HVE_IP_H
#define HVE_IP_H

#include <pbc/pbc.h>

#include "schemas/utils.h"

typedef struct {
  int l;
  element_t ***B;
  element_t g_T;
}* mpk_t;

typedef struct {
  int l;
  element_t ***C;
}* msk_t;

typedef struct {
  mpk_t public;
  msk_t private;
  element_t g;
}* setup_t;

typedef struct {
  element_t c;
  element_t **ci;
}* ciphertext_t;

typedef struct {
  element_t *k;
}* key_t;

pairing_t* load_pairing(char *params_path);
setup_t setup(pairing_t* pairing, int l);
ciphertext_t encrypt(pairing_t* pairing, mpk_t public, int x[], element_t *m);
key_t keygen(pairing_t* pairing, msk_t private, int y[]);
element_t * decript(pairing_t* pairing, ciphertext_t ct, key_t key);

#endif
