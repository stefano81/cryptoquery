#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>

#include <sqlite3.h>

#include <pbc/pbc.h>
#include "schemas/utils.h"

#include "../schemas/hve_ip.h"

int main(int argc, char ** argv) {
  sqlite3 *db;
  sqlite3_open(argv[1], &db);

  sqlite3_stmt *get_plain_data, *insert_ct, *insert_m, *insert_param;
  sqlite3_prepare_v2(db, "select rowid, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9 from plain_data", -1, &get_plain_data, NULL);
  sqlite3_prepare_v2(db, "insert into encrypted_data (id, ciphertext) values (:id, :ct)", -1, &insert_ct, NULL);
  sqlite3_prepare_v2(db, "insert into test_data(id, m) values (:id, :m)", -1, &insert_m, NULL);
  sqlite3_prepare_v2(db, "insert into params (name, param) values (:name, :param)", -1, &insert_param, NULL);

  pairing_t * pairing = load_pairing(argv[2]);
  setup_t * hve = setup(pairing, 10);

  int ret = sqlite3_bind_text(insert_param, sqlite3_bind_parameter_index(insert_param, ":name"), "curve_path", -1, NULL);
  ret = sqlite3_bind_text(insert_param, sqlite3_bind_parameter_index(insert_param, ":param"), argv[2], -1, NULL);

  if (SQLITE_DONE != sqlite3_step(insert_param)) {
    fprintf(stderr, "Error inserting curve path\n");

    return 1;
  }

  unsigned int BUFFERSIZE = 1024;
  unsigned char *buffer = malloc(sizeof(unsigned char) * BUFFERSIZE);
  unsigned char *buff;

  sqlite3_reset(insert_param);
  int dim = serialize_mpk(&buff, hve->public);
  ret = sqlite3_bind_text(insert_param, sqlite3_bind_parameter_index(insert_param, ":name"), "public", -1, NULL);
  ret = sqlite3_bind_blob(insert_param, sqlite3_bind_parameter_index(insert_param, ":param"), buff, dim, NULL);

  if (SQLITE_DONE != sqlite3_step(insert_param)) {
    fprintf(stderr, "Error inserting public\n");

    return 1;
  }
  free(buff);

  sqlite3_reset(insert_param);
  dim = serialize_msk(&buff, hve->private);
  ret = sqlite3_bind_text(insert_param, sqlite3_bind_parameter_index(insert_param, ":name"), "private", -1, NULL);

  ret = sqlite3_bind_blob(insert_param, sqlite3_bind_parameter_index(insert_param, ":param"), buff, dim, NULL);

  if (SQLITE_DONE != sqlite3_step(insert_param)) {
    fprintf(stderr, "Error inserting private\n");

    return 1;
  }

  free(buff);

  int j = 0;
  unsigned int X[10];
  element_t m;

  while (SQLITE_ROW == sqlite3_step(get_plain_data)) {
    int id = sqlite3_column_int(get_plain_data, 10);

    printf("row[i] {id == %d}\n", j++, id);

    for (int i = 0; i < 10; ++i) {
      X[i] = sqlite3_column_int(get_plain_data, 10 - i);
    }

    printf("X read\n"); 

    ciphertext_t *ct = encrypt(pairing, hve->public, X, &m);

    printf("ciphertext created\n");

    sqlite3_reset(insert_ct);
    sqlite3_bind_int(insert_ct,
		     sqlite3_bind_parameter_index(insert_param, ":id"),
		     id);
    dim = serialize_ct(&buff, ct);

    sqlite3_bind_blob(insert_ct, sqlite3_bind_parameter_index(insert_ct, ":ct"), buff, dim, SQLITE_TRANSIENT);
    sqlite3_step(insert_ct);
    free(buff);

    printf("ct stored\n");

    sqlite3_reset(insert_m);
    sqlite3_bind_int(insert_m,
		     sqlite3_bind_parameter_index(insert_m, ":id"),
		     id);
    dim = element_to_bytes(buffer, m);
    sqlite3_bind_blob(insert_m, sqlite3_bind_parameter_index(insert_m, ":m"), buffer, dim, SQLITE_TRANSIENT);
    sqlite3_step(insert_m);
    element_free(m);

    printf("m stored\n");
  }    

  return 0;
}
