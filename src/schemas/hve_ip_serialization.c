#include "hve_ip.h"
#include <string.h>
#include <stdlib.h>
#include <pbc/pbc.h>


int serialize_ct(unsigned char ** buffer, ciphertext_t * ct) {
  int size = sizeof(unsigned int) + element_length_in_bytes(ct->c) + (ct->l * element_length_in_bytes(ct->ci[0][0]));
  unsigned char * tbuf, * buff = tbuf = *buffer = malloc(size);
  unsigned int l1 = htonl(ct->l);

  memcpy(buff, &l1, sizeof(unsigned int));
  tbuf += sizeof(unsigned int);

  tbuf += element_to_bytes(tbuf, ct->c);

  for (int i = 0; i <= ct->l; ++i)
    for (int j = 0; j < 3; ++j)
      tbuf += element_to_bytes(tbuf, ct->ci[i][j]);

  return size;
}

ciphertext_t * deserialize_ct(unsigned char *buffer) {
  ciphertext_t *ct = malloc(sizeof(ciphertext_t));

  unsigned int l1;
  memcpy(&l1, buffer, sizeof(unsigned int));
  ct->l = ntohl(l1);

  buffer += sizeof(unsigned int);

  buffer += element_from_bytes(ct->c, buffer);
  ct->ci = malloc(sizeof(element_t *) * ct->l);
  for (int i = 0; i <= ct->l; ++i) {
    ct->ci[i] = malloc(sizeof(element_t) * 3);
    for (int j = 0; j < 3; ++j)
      buffer += element_from_bytes(ct->ci[i][j], buffer);
  }

  return ct;
}

int serialize_mpk(unsigned char ** buffer, mpk_t *public) {
  int size = sizeof(unsigned short) + ((public->l + 1) * 3 * 3 * element_length_in_bytes(public->B[0][0][0])) + element_length_in_bytes(public->gT);
  unsigned char *tbuff = *buffer = malloc(size);

  unsigned short l1 = htons(public->l);
  memcpy(tbuff, &l1, sizeof(unsigned short));
  tbuff += sizeof(unsigned short);
  for (int t = 0; t <= public->l; ++t)
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
	tbuff += element_to_bytes(tbuff, public->B[t][i][j]);
  element_to_bytes(tbuff, public->gT);

  return size;
}

mpk_t* deserialize_mpk(unsigned char * buffer) {
  mpk_t* public = malloc(sizeof(mpk_t));
  
  unsigned short l1;
  memcpy(&l1, buffer, sizeof(unsigned short));
  buffer += sizeof(unsigned short);

  public->l = ntohs(l1);
  public->B = malloc(sizeof(element_t **) * (public->l + 1));
  for (int t = 0; t <= public->l; ++t) {
    public->B[t] = malloc(sizeof(element_t *) * 3);
    for (int i = 0; i < 3; ++i) {
      public->B[t][i] = malloc(sizeof(element_t) * 3);
      for (int j = 0; j < 3; ++j)
	buffer += element_from_bytes(public->B[t][i][j], buffer);
    }
  }
  element_from_bytes(public->gT, buffer);

  return public;
}

int serialize_msk(unsigned char ** buffer, msk_t *private) {
  int size = sizeof(unsigned short) + ((private->l + 1) * 3 * 3 * element_length_in_bytes(private->C[0][0][0]));
  unsigned char *tbuff = *buffer = malloc(size);

  unsigned short l1 = htons(private->l);
  memcpy(tbuff, &l1, sizeof(unsigned short));
  tbuff += sizeof(unsigned short);
  for (int t = 0; t <= private->l; ++t)
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
	tbuff += element_to_bytes(tbuff, private->C[t][i][j]);

  return size;
}

msk_t *deserialize_msk(unsigned char * buffer) {
  msk_t *private = malloc(sizeof(msk_t));

  unsigned short l1;
  memcpy(&l1, buffer, sizeof(unsigned short));
  buffer += sizeof(unsigned short);

  private->l = ntohs(l1);
  private->C = malloc(sizeof(element_t **) * (private->l + 1));

  for (int t = 0; t <= private->l; ++t) {
    private->C[t] = malloc(sizeof(element_t *) * 3);
    for (int i = 0; i < 3; ++i) {
      private->C[t][i] = malloc(sizeof(element_t) * 3);
      for (int j = 0; j < 3; ++j)
	buffer += element_from_bytes(private->C[t][i][j], buffer);
    }
  }

  return private;
}

int serialize_key(unsigned char ** buffer, dkey_t *key) {
  int size = sizeof(unsigned long) + sizeof(unsigned short) + (element_length_in_bytes(key->k[0][0]) * (key->l + 1));
  unsigned char *tbuff  = *buffer = malloc(size);

  // TODO!!!

  return size;
}

dkey_t *deserialize_key(unsigned char * buffer) {
  // TODO!!!
}
