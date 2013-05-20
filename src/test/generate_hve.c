#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <math.h>

#include "../schemas/hve_ip.h"
#include "../schemas/utils.h"

// usage: generatehve database_file, param_file, column_number, data_row_number
// assumes column numerical
// generates random data
// creates 5 table: plain text, ciphertex_AES, key_HVE, key_store, params
// key_store[0] = master_public_key;
// key_store[1] = master_private_key;
int main(int argc, char *argv[]) {
  if (5 != argc) {
    fprintf(stderr, "ERROR!: %d\n", argc);
    return 1;
  }

  int ncolumn = (int) strtol(argv[3], NULL, 10);

  if (0 >= ncolumn) {
    fprintf(stderr, "Invalid number of columns: %s\n", argv[3]);

    return 1;
  }

  int nrow = (int) strtol(argv[4], NULL, 10);
  if (0 >= nrow) {
    fprintf(stderr, "Invalid number of rows: %s\n", argv[4]);

    return 1;
  }

  sqlite3 * db;

  if (SQLITE_OK != sqlite3_open(argv[1], &db)) {
    fprintf(stderr, "Unable to open the database file at %s\n", argv[1]);

    return 1;
  }

  int nchar = (int) ceil(log10(ncolumn)) + (0 == ncolumn % 10 ? 1 : 0);
  fprintf(stderr, "%d %d\n", ncolumn, nchar);

  char * create_query = malloc(sizeof("CREATE TABLE IF NOT EXISTS plain (id integer") + (ncolumn * (sizeof(", column integer") + (nchar * sizeof(char)))) + sizeof(")") + 1);
  int p = sprintf(create_query, "CREATE TABLE IF NOT EXISTS plain (id integer");

  char buff[1024];
  int p2 = 0;

  sprintf(buff, ", column%%.%dd integer", nchar);
  for (int i = 0; i < ncolumn; ++i) {
    p += sprintf(create_query+p, buff, i);
  }

  p += sprintf(create_query+p, ");");

  fprintf(stderr, "strlen(create_query) -> %d\n", strlen(create_query));

  sqlite3_stmt * stmt;

  int e = sqlite3_prepare(db, create_query, -1, &stmt, NULL);
  if (SQLITE_OK != e) {
    fprintf(stderr, "Error preparing table plain: %d\n", e);

    return 1;
  }

  if (SQLITE_DONE != sqlite3_step(stmt)) {
    fprintf(stderr, "Error creating table plain\n");

    return 1;
  }

  if (SQLITE_OK !=  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS ciphertext (id integer, cipher blob)", NULL, NULL, NULL)) {
    fprintf(stderr, "Error creating table ciphertext\n");

    return 1;
  }
  if (SQLITE_OK !=  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS key (id integer, key blob)", NULL, NULL, NULL)) {
    fprintf(stderr, "Error creating table key\n");

    return 1;
  }
  if (SQLITE_OK !=  sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS param (key integer, val blob)", NULL, NULL, NULL)) {
    fprintf(stderr, "Error creating table param\n");

    return 1;
  }
  
  // init pairing
  pairing_t * pairing = load_pairing(argv[2]);

  // init HVE
  setup_t mks = setup(pairing, ncolumn);

  unsigned int * vals = malloc(sizeof(unsigned int) * ncolumn);
  for (int i = 0, p = 0; i < ncolumn; ++i) {
    
  }
  for (int i = 0; i < nrow; ++i) {
    for (int j = 0; j < ncolumn; ++j) {
      
    }
  }
  free(vals);


  sqlite3_close(db);

  return 0;
}
