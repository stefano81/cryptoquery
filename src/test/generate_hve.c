#include <stdio.h>
#include <stdlib.h>

#include "../schemas/hve.h"

// usage: database_file, param_file, column_number
// assumes column numerical
// generates random data
// creates 3 table: plain text, ciphertex_AES, key_HVE
int main(int argc, char *argv[]) {
  if (4 != argc) {
    fprintf(stderr, "ERROR!");
    return 1;
  }

  sqlite3 db;

  return 0;
}
