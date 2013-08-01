#include "hve_ip.h"
#include <string.h>
#include <stdlib.h>
#include <pbc/pbc.h>

const unsigned int BUFSIZE = 1024 * 1024;

int serialize_ct(unsigned char ** buffer, ciphertext_t * ct) {
  unsigned char buff[BUFSIZE];
  size_t size = 0;
  unsigned int l1 = htonl(ct->l);

  memcpy(buff, &l1, sizeof(unsigned int));
  size += sizeof(unsigned int);

  size += element_to_bytes(buff + size, ct->c);

  for (int i = 0; i <= ct->l; ++i)
    for (int j = 0; j < 3; ++j)
      size += element_to_bytes(buff + size, ct->ci[i][j]);

  *buffer = malloc(sizeof(unsigned char) * size);
  memcpy(*buffer, buff, size);

  return size;
}

ciphertext_t * deserialize_ct(unsigned char *buffer, pairing_t * pairing) {
  ciphertext_t *ct = malloc(sizeof(ciphertext_t));

  unsigned int l1;
  memcpy(&l1, buffer, sizeof(unsigned int));
  ct->l = ntohl(l1);

  buffer += sizeof(unsigned int);

  element_init_GT(ct->c, *pairing);

  buffer += element_from_bytes(ct->c, buffer);
  ct->ci = malloc(sizeof(element_t *) * ct->l);
  for (int i = 0; i <= ct->l; ++i) {
    ct->ci[i] = malloc(sizeof(element_t) * 3);
    for (int j = 0; j < 3; ++j) {
      element_init_G2(ct->ci[i][j], *pairing);
      buffer += element_from_bytes(ct->ci[i][j], buffer);
    }
  }

  return ct;
}

int serialize_mpk(unsigned char ** buffer, mpk_t *public) {
  unsigned char buff[BUFSIZE];
  size_t size = 0;

  unsigned short l1 = htons(public->l);
  memcpy(buff, &l1, sizeof(unsigned short));
  size += sizeof(unsigned short);
  size += element_to_bytes(buff + size, public->gT);
  for (int t = 0; t <= public->l; ++t)
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
	size += element_to_bytes(buff + size, public->B[t][i][j]);

  *buffer = malloc(sizeof(unsigned char) * size);
  memcpy(*buffer, buff, size);

  return size;
}

mpk_t* deserialize_mpk(unsigned char * buffer, pairing_t * pairing) {
  mpk_t* public = malloc(sizeof(mpk_t));
  
  unsigned short l1;
  memcpy(&l1, buffer, sizeof(unsigned short));
  buffer += sizeof(unsigned short);

  public->l = ntohs(l1);

  element_init_GT(public->gT, *pairing);
  buffer += element_from_bytes(public->gT, buffer);

  public->B = malloc(sizeof(element_t **) * (public->l + 1));
  for (int t = 0; t <= public->l; ++t) {
    public->B[t] = malloc(sizeof(element_t *) * 3);
    for (int i = 0; i < 3; ++i) {
      public->B[t][i] = malloc(sizeof(element_t) * 3);
      for (int j = 0; j < 3; ++j) {
	element_init_G1(public->B[t][i][j], *pairing);
	buffer += element_from_bytes(public->B[t][i][j], buffer);
      }
    }
  }

  return public;
}

int serialize_msk(unsigned char ** buffer, msk_t *private) {
  unsigned char buff[BUFSIZE];
  int size = 0;

  unsigned short l1 = htons(private->l);
  memcpy(buff, &l1, sizeof(unsigned short));
  size += sizeof(unsigned short);
  for (int t = 0; t <= private->l; ++t)
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
	size += element_to_bytes(buff + size, private->C[t][i][j]);

  *buffer = malloc(sizeof(unsigned char) * size);
  memcpy(*buffer, buff, size);

  return size;
}

msk_t *deserialize_msk(unsigned char * buffer, pairing_t * pairing) {
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
      for (int j = 0; j < 3; ++j) {
	element_init_G2(private->C[t][i][j], *pairing);
	buffer += element_from_bytes(private->C[t][i][j], buffer);
      }
    }
  }

  return private;
}

int serialize_key(unsigned char ** buffer, dkey_t *key) {
  int size = 0;
  unsigned char buff[BUFSIZE];
  unsigned short l1;
  unsigned long S1;

  l1 = htons(key->l);
  memcpy(buff, &l1, sizeof(unsigned short));
  size += sizeof(unsigned short);

  S1 = htonl(key->Sn);
  memcpy(buff + size, &S1, sizeof(unsigned long));
  size += sizeof(unsigned long);

  for (int i = 0; i < key->Sn; ++i) {
    printf("key->S[%d] := %d\n", i, key->S[i]);
    S1 = htonl(key->S[i]);
    memcpy(buff + size, &S1, sizeof(unsigned long));
    size += sizeof(unsigned long);
  }

  for (int i = 0; i <= key->l; ++i) {
    if (NULL == key->k)
      continue;
    for (int j = 0; j < 3; ++j) {
      size += element_to_bytes(buff + size, key->k[i][j]);
    }
  }
  
  *buffer = malloc(sizeof(unsigned char) * size);
  memcpy(*buffer, buff, size);

  return size;
}

dkey_t *deserialize_key(unsigned char * buffer, pairing_t * pairing) {
  unsigned short l1;
  unsigned long S1;
  dkey_t * key = malloc(sizeof(dkey_t));

  memcpy(&l1, buffer, sizeof(unsigned short));
  buffer += sizeof(unsigned short);

  key->l = ntohs(l1);

  memcpy(&S1, buffer, sizeof(unsigned long));
  buffer += sizeof(unsigned long);

  key->Sn = ntohl(S1);

  key->S = malloc(sizeof(unsigned long) * key->Sn);
  for (int i = 0; i < key->Sn; ++i) {
    memcpy(&S1, buffer, sizeof(unsigned long));
    buffer += sizeof(unsigned long);

    key->S[i] = ntohl(S1);
  }

  key->k = malloc(sizeof(element_t *) * key->Sn);
  key->k[0] = malloc(sizeof(element_t) * 3);
  for (int j = 0; j < 3; ++j) {
    element_init_G2(key->k[0][j], *pairing);
    buffer += element_from_bytes(key->k[0][j], buffer);
  }

  for (int i = 1, sc = 0; i <= key->l; ++i) {
    if ((i - 1) == key->S[sc]) {
      sc++;
      key->k[i] = malloc(sizeof(element_t) * 3);
      for (int j = 0; j < 3; ++j) {
	element_init_G2(key->k[i][j], *pairing);
	buffer += element_from_bytes(key->k[i][j], buffer);
      }
    } else {
      key->k[i] = NULL;
    }
  }

  return key;
}
