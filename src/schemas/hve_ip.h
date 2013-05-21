#ifndef HVE_IP_H
#define HVE_IP_H

#include <pbc/pbc.h>
#include <arpa/inet.h>
#include "schemas/utils.h"

typedef struct {
  unsigned short l;
  element_t ***B;
  element_t g_T;
}* mpk_t;

typedef struct {
  unsigned short l;
  element_t ***C;
}* msk_t;

typedef struct {
  mpk_t public;
  msk_t private;
  element_t g;
}* setup_t;

typedef struct {
  unsigned short l;
  element_t c;
  element_t **ci;
}* ciphertext_t;

typedef struct {
  element_t *k;
}* dkey_t;

pairing_t* load_pairing(char *params_path);
setup_t setup(pairing_t* pairing, int l);
ciphertext_t encrypt(pairing_t* pairing, mpk_t public, unsigned int x[], element_t *m);
dkey_t keygen(pairing_t* pairing, msk_t private, int y[]);
element_t * decript(pairing_t* pairing, ciphertext_t ct, dkey_t key);

int serialize_ct(void ** buffer, ciphertext_t ct);
int serialize_mpk(void ** buffer, mpk_t pulbic);
int serialize_msk(void ** buffer, msk_t private);
int serialize_key(void ** buffer, dkey_t k);

#endif
