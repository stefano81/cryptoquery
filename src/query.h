#ifndef QUERY_H
#define QUERY_H

#include <pbc.h>
#include <stdio.h>

typedef struct {
  pairing_t pairing;
  element_t g;
} group_info;

typedef struct {
  element_t * * T;
  unsigned int l;
  element_t omega;
} mpk;

typedef struct {
  element_t * * t;
  unsigned int l;
} msk;

typedef struct {
  group_info * info;
  mpk * mpk;
  msk * msk;
} setup_output;

typedef struct {
  element_t * X;
  element_t M;
} cipher;

typedef struct {
  element_t * tk;
  unsigned int * S;
  unsigned int Ssize;
} public_key;

typedef struct {
  element_t m;
} message;


setup_output * setup(int lambda, char * path_to_param);

cipher * enc(message *m, unsigned char *x, group_info *info, mpk *public);
public_key * keygen(int *y, group_info *info, msk *secret);

message * dec(cipher *ct, public_key *key, group_info *info);

int check(message *m1, message *m2);
#endif
