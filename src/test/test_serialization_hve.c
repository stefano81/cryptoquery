#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>

#include <sqlite3.h>

#include <assert.h>

#include <pbc/pbc.h>
#include "schemas/utils.h"

#include "../schemas/hve_ip.h"

void verify_ct(ciphertext_t *orig, ciphertext_t *new) {
  assert(orig->l == new->l);
  assert(0 == element_cmp(orig->c, new->c));
  for (int i = 0; i < orig->l; ++i) {
    for (int j = 0; j < 3; ++j) {
      assert(0 == element_cmp(orig->ci[i][j], new->ci[i][j]));
    }
  }
}

void verify_msk(msk_t *orig, msk_t *new) {
  assert(orig->l == new->l);

  for (int i = 0; i <= orig->l; ++i) {
    for (int j = 0; j < 3; ++j) {
      for (int k = 0; k < 3; ++k) {
	assert(0 == element_cmp(orig->C[i][j][k], new->C[i][j][k]));
      }
    }
  }
}

void verify_mpk(mpk_t *orig, mpk_t *new) {
  assert(orig->l == new->l);

  assert(0 == element_cmp(orig->gT, new->gT));

  for (int i = 0; i <= orig->l; ++i) {
    for (int j = 0; j < 3; ++j) {
      for (int k = 0; k < 3; ++k) {
	assert(0 == element_cmp(orig->B[i][j][k], new->B[i][j][k]));
      }
    }
  }
}

void verify_key(dkey_t *orig, dkey_t *new) {
  assert(orig->Sn == new->Sn);
  assert(orig->l == new->l);

  for (int i = 0; i < orig->Sn; ++i) {
    printf("%d - %d %d\n", i, orig->S[i], new->S[i]);
    assert(orig->S[i] == new->S[i]);
  }

  for (int i = 0; i < orig->l; ++i) {
    for (int j = 0; j < 3; ++j) {
      assert(0 == element_cmp(orig->k[i][j], new->k[i][j]));
    }
  }
}

void test_save_and_recover(setup_t *hve, pairing_t *pairing) {
  element_t m;
  int X[10] = {1,2,3,4,5,6,7,8,9,10};
  int Y[10] = {1,2,3,4,5,6,7,8,9,10};

  unsigned char *publicB, *privateB, *ctB;

  serialize_mpk(&publicB, hve->public);
  serialize_msk(&privateB, hve->private);

  mpk_t *public = deserialize_mpk(publicB, pairing);

  verify_mpk(public, hve->public);

  ciphertext_t *ct1 = encrypt(pairing, public, X, &m);

  serialize_ct(&ctB, ct1);

  ciphertext_t *ct2 = deserialize_ct(ctB, pairing);

  verify_ct(ct2, ct1);

  element_t dm;
  
  //  assert(0 == element_cmp
}

int main(int argc, char ** argv) {
  sqlite3 *db;

  unsigned int BUFFERSIZE = 1024;
  unsigned char *buffer = malloc(sizeof(unsigned char) * BUFFERSIZE);
  unsigned char *buff;
  int j = 0;
  unsigned int X[10];

  element_t m, t1, t2;

  sqlite3_stmt *get_plain_data;

  pairing_t * pairing = load_pairing(argv[2]);

  sqlite3_open(argv[1], &db);
  sqlite3_prepare_v2(db, "select rowid, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9 from plain_data", -1, &get_plain_data, NULL);

  element_init_G1(t1, *pairing);
  element_random(t1);

  int size = element_to_bytes(buffer, t1);

  printf("%d\n", size);
  element_init_G1(t2, *pairing);

  element_set0(t2);

  assert(element_cmp(t1, t2));

  element_from_bytes(t2, buffer);

  assert(!element_cmp(t1, t2));

  setup_t * hve = setup(pairing, 10);

  test_save_and_recover(hve, pairing);

  int dim = serialize_mpk(&buff, hve->public);
  mpk_t *public2 = deserialize_mpk(buff, pairing);

  verify_mpk(hve->public, public2);

  dim = serialize_msk(&buff, hve->private);
  msk_t *private2 = deserialize_msk(buff, pairing);

  verify_msk(hve->private, private2);

  while (SQLITE_ROW == sqlite3_step(get_plain_data)) {
    int id = sqlite3_column_int(get_plain_data, 10);

    printf("row[i] {id == %d}\n", j++, id);

    for (int i = 0; i < 10; ++i) {
      X[i] = sqlite3_column_int(get_plain_data, 10 - i);
    }

    printf("X read\n"); 

    ciphertext_t *ct = encrypt(pairing, hve->public, X, &m);

    printf("ciphertext created\n");

    dim = serialize_ct(&buff, ct);

    ciphertext_t *ct2 = deserialize_ct(buff, pairing);

    verify_ct(ct, ct2);

    dkey_t *key1 = keygen(pairing, hve->private, X);

    dim = serialize_key(&buff, key1);
    dkey_t *key2 = deserialize_key(buff, pairing);

    verify_key(key1, key2);

    return 0;
  }
  return 0;
}
