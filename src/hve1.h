#ifndef HVE1_H
#define HVE1_H

#include <pbc.h>
#include <stdio.h>

typedef struct {
  pairing_t pairing;
  element_t g;
} group;

typedef struct {
  element_t T;
  element_t V;
  element_t R;
  element_t M;
} touple_public;

typedef struct {
  element_t Y;
  unsigned int n;
  touple_public VT[];
} master_public_key;

typedef struct {
  element_t t;
  element_t v;
  element_t r;
  element_t m;
} touple_private;

typedef struct {
  element_t y;
  unsigned int n;
  touple_private vt[];
} master_private_key;

typedef struct {
  group * I;
  master_public_key *public;
  master_private_key *private;
} setup_output;

typedef struct {
  element_t X;
  element_t W;
} couple_ciphertext;

typedef struct {
  element_t omega;
  element_t C0;
  unsigned int n;
  ciphertext_couple C[];
} ciphertext;

typedef struct {
  
} key;

setup_output * hve1_setup(const unsigned int k, const unsigned int n, char path_name[]);
ciphertext * hve1_enc(const master_public_key *pk, const int x, element_t *m);
key * hve1_keygen(const master_private_key *msk, const int y);

#endif
