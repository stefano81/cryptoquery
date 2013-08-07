#include <stdio.h>
#include <pbc/pbc.h>
#include "../schemas/hve_ip.h"
#include "../schemas/utils.h"

#include <sqlite3.h>

void compute_sizes(pairing_t *pairing) {
  for (int i = 10; i <= 200; i += 10) {
    int *X = malloc(sizeof(int) * i);

    for (int j = 0; j < i; ++j) {
      X[j] = j + 1;
    }
    unsigned char * buff;

    printf("o %d %d\n", i, ( sizeof(int) * i ));

    setup_t *hve = setup(pairing, i);
    dkey_t *key = keygen(pairing, hve->private, X);

    printf("k %d %d\n", i, serialize_key(&buff, key));

    free(buff);

    element_t m;
    ciphertext_t *ct = encrypt(pairing, hve->public, X, &m);

    printf("c %d %d\n", i, serialize_ct(&buff, ct));

    free(buff);
  }
}

setup_t * load_or_create(sqlite3 *db, pairing_t *pairing, int l) {
  sqlite3_stmt *public = NULL, *private = NULL, *g = NULL, *insert;
  setup_t * out;

  sqlite3_prepare_v2(db, "select param from params where name = 'public'", -1, &public, NULL);

  if (SQLITE_ROW == sqlite3_step(public)) {
    out = malloc(sizeof(setup_t));

    sqlite3_prepare_v2(db, "select param from params where name = 'private'", -1, &private, NULL);

    if (SQLITE_ROW == sqlite3_step(private)) {
      

      sqlite3_prepare_v2(db, "select param from params where name = 'g'", -1, &g, NULL);
    }
    
  } else {
    out = setup(pairing, l);

    unsigned char *buff, buff2[1024];

    sqlite3_prepare_v2(db, "insert into param (name, param) values (:name, :param)", -1, &insert, NULL);

    if (SQLITE_ROW == sqlite3_step(public)) {
      sqlite3_prepare_v2(db, "select param from params where name = 'private'", -1, &private, NULL);

      if (SQLITE_ROW == sqlite3_step(private)) {

	sqlite3_prepare_v2(db, "select param from params where name = 'g'", -1, &g, NULL);
      }
    }

  }
  sqlite3_finalize(public);
  sqlite3_finalize(private);
  sqlite3_finalize(g);
  sqlite3_finalize(insert);

  return out;
}

void compute_sizes_given_attributen(pairing_t * pairing, char *dbname, int nattribute, int step) {
  sqlite3 *db;

  sqlite3_open_v2(dbname, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

  setup_t * hve = load_or_create(&db, pairing, nattribute);

  sqlite3_stmt *insert_plain, *insert_enc, *insert_test;
}

int main(int argc, char *argv[]) {
  pairing_t *pairing = load_pairing(argv[1]);

  if (2 == argc ) {
    compute_sizes(pairing);
  } else if (5 == argc) {
    compute_sizes_given_attributen(pairing, argv[2], atoi(argv[3]), atoi(argv[4]));
  }
}
