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

int die(sqlite3 *db) {
  fprintf(stderr, "ERROR: %d %s\n", sqlite3_errcode(db), sqlite3_errmsg(db));

  exit(1);
}

setup_t * load_or_create(sqlite3 *db, pairing_t *pairing, int l) {
  sqlite3_stmt *public = NULL, *private = NULL, *g = NULL, *insert = NULL;
  setup_t * out;

  SQLITE_OK == sqlite3_prepare_v2(db, "select param from params where name = 'public'", -1, &public, NULL) || die(db);

  if (SQLITE_ROW == sqlite3_step(public)) {
    out = malloc(sizeof(setup_t));

    out->public = deserialize_mpk((unsigned char *) sqlite3_column_blob(public, 1), pairing);

    SQLITE_OK == sqlite3_prepare_v2(db, "select param from params where name = 'private'", -1, &private, NULL) || die(db);

    if (SQLITE_ROW == sqlite3_step(private)) {
      out->private = deserialize_msk((unsigned char *) sqlite3_column_blob(private, 1), pairing);

      SQLITE_OK == sqlite3_prepare_v2(db, "select param from params where name = 'g'", -1, &g, NULL) || die(db);

      if (SQLITE_ROW == sqlite3_step(g)) {
	element_init_GT(out->g, *pairing);
	element_from_bytes(out->g, (unsigned char *) sqlite3_column_blob(g, 1));
      }
    }
    
  } else {
    out = setup(pairing, l);

    unsigned char *buff, buff2[1024];

    SQLITE_OK == sqlite3_prepare_v2(db, "insert into params (name, param) values (:name, :param)", -1, &insert, NULL) || die(db);

    int dim = serialize_mpk(&buff, out->public);
    sqlite3_bind_text(insert, sqlite3_bind_parameter_index(insert, ":name"), "public", -1, NULL);
    sqlite3_bind_blob(insert, sqlite3_bind_parameter_index(insert, ":param"), buff, dim, NULL);
    sqlite3_step(insert);
    free(buff);
    sqlite3_reset(insert);

    dim = serialize_msk(&buff, out->private);
    sqlite3_bind_text(insert, sqlite3_bind_parameter_index(insert, ":name"), "private", -1, NULL);
    sqlite3_bind_blob(insert, sqlite3_bind_parameter_index(insert, ":param"), buff, dim, NULL);
    sqlite3_step(insert);
    free(buff);
    sqlite3_reset(insert);

    /*    dim = element_to_bytes(buff2, out->g);
    sqlite3_bind_text(insert, sqlite3_bind_parameter_index(insert, ":name"), "g", -1, NULL);
    sqlite3_bind_blob(insert, sqlite3_bind_parameter_index(insert, ":param"), buff2, dim, NULL);
    sqlite3_step(insert);*/
  }

  sqlite3_finalize(public);
  sqlite3_finalize(private);
  sqlite3_finalize(g);
  sqlite3_finalize(insert);

  return out;
}

void compute_sizes_given_attributen(pairing_t * pairing, char *dbname, int nattribute, int step) {

  fprintf(stderr,"called for %d attributes and %d tuples\n", nattribute, step);
  sqlite3 *db;

  sqlite3_open_v2(dbname, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

  setup_t * hve = load_or_create(db, pairing, nattribute);

  sqlite3_stmt *insert_plain, *insert_enc, *insert_test;

  unsigned int *X = malloc(sizeof(unsigned int) * nattribute);
  element_t m;
  unsigned char buffer[1024], *buff;

  
  SQLITE_OK == sqlite3_prepare_v2(db, "insert into plain_data (c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19) values (:c0, :c1, :c2, :c3, :c4, :c5, :c6, :c7, :c8, :c9, :c10, :c11, :c12, :c13, :c14, :c15, :c16, :c17, :c18, :c19)", -1, &insert_plain, NULL) || die(db);
  sqlite3_prepare_v2(db, "insert into encrypted_data (id, ct) values (:id, :ct)", -1, &insert_enc, NULL);
  sqlite3_prepare_v2(db, "insert into test_data(id, m) values (:id, :m)", -1, &insert_test, NULL);

  for (int i = 0; i < step; ++i) {
    fprintf(stderr,"%d\n", i);
    // create X
    SQLITE_OK == sqlite3_reset(insert_plain) || die(db);
    for (int j = 0; j < nattribute; ++j) {
      X[j] = random();
      fprintf(stderr, "X[%d] = %u\n", j, X[j]);
      SQLITE_OK == sqlite3_bind_int(insert_plain, j+1, X[j]) || die(db);
    }
    fprintf(stderr,"insert\n");
    SQLITE_DONE == sqlite3_step(insert_plain) || die(db);
    
    sqlite3_int64 id = sqlite3_last_insert_rowid(db);
    ciphertext_t *ct = encrypt(pairing, hve->public, X, &m);
    int size = serialize_ct(&buff, ct);
    SQLITE_OK == sqlite3_reset(insert_enc) || die(db);
    sqlite3_bind_int64(insert_enc, sqlite3_bind_parameter_index(insert_enc, ":id"), id);
    sqlite3_bind_blob(insert_enc, sqlite3_bind_parameter_index(insert_enc, ":ct"), buff, size, SQLITE_TRANSIENT);
    fprintf(stderr,"insert\n");
    SQLITE_DONE == sqlite3_step(insert_enc) || die(db);

    free(buff);

    size = element_to_bytes(buffer, m);
    sqlite3_reset(insert_test);
    sqlite3_bind_int64(insert_test, sqlite3_bind_parameter_index(insert_test, ":id"), id);
    sqlite3_bind_blob(insert_test, sqlite3_bind_parameter_index(insert_test, ":m"), buffer, size, SQLITE_TRANSIENT);
    fprintf(stderr,"insert\n");
    SQLITE_DONE == sqlite3_step(insert_test) || die(db);

    //free_ct(ct);
  }
}

int main(int argc, char *argv[]) {
  pairing_t *pairing = load_pairing(argv[1]);

  if (2 == argc ) {
    compute_sizes(pairing);
  } else if (5 == argc) {
    compute_sizes_given_attributen(pairing, argv[2], 20 /*atoi(argv[3])*/, atoi(argv[4]));
  }
}
