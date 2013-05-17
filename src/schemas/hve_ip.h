#ifndef HVE_IP_H
#define HVE_IP_H

#include <pbc/pbc.h>

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
}* setup_t;

typedef struct {
  element_t c;
  element_t **ci;
}* ciphertext_t;

typedef struct {
  element_t *k;
}* key_t;

pairing_t* load_pairing(const char *params_path);
setup_t setup(const pairing_t* pairing, const int l);
ciphertext_t encrypt(const pairing_t* pairing, const mpk_t public, const int x[], element_t *m);
key_t keygen(const pairing_t* pairing, const msk_t private, const int y[]);
element_t * decript(const pairing_t* pairing, const ciphertext_t ct, const key_t key);

#endif
