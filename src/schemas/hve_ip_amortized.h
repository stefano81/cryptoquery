#ifndef HVE_IP_AMM_H
#define HVE_IP_AMM_H

#include <pbc/pbc.h>

typedef struct {
  unsigned int l;
  element_t ***B;
  element_t gT;
} mpk_t;

typedef struct {
  unsigned int l;
  element_t ***C;
} msk_t;

typedef struct {
  mpk_t *public;
  msk_t *private;
  element_t g;
} setup_t;

typedef struct {
  unsigned int l;
  unsigned int n;
  element_t *c;
  element_t **ci;
  element_t ***cj;
} ciphertext_t;

typedef struct {
  unsigned long Sn;
  unsigned long *S;
  element_t **k;
  element_t *ks;
  element_t *kzs;
  unsigned short l;
} dkey_t;

setup_t* setup_amortized(pairing_t* pairing, int l);
ciphertext_t * encrypt_amortized(pairing_t* pairing, mpk_t* public, int x[], unsigned int n, int * xj, element_t *m);
dkey_t* keygen_amortized(pairing_t* pairing, msk_t* private, int y[]);
element_t* decrypt_amortized(pairing_t* pairing, ciphertext_t* ct, dkey_t* key);

/*
int serialize_ct(unsigned char ** buffer, ciphertext_t* ct);
int serialize_mpk(unsigned char ** buffer, mpk_t* public);
int serialize_msk(unsigned char ** buffer, msk_t* private);
int serialize_key(unsigned char ** buffer, dkey_t* k);

ciphertext_t* deserialize_ct(unsigned char * buffer);
mpk_t* deserialize_mpk(unsigned char * buffer);
msk_t* deserialize_msk(unsigned char * buffer);
dkey_t* deserialize_key(unsigned char * buffer);
*/
#endif
