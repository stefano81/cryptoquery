#include "hve_ip.h"
#include <stdio.h>
#include <stdlib.h>

pairing_t * load_pairing(const char *params_path) {
  char param[1024];
  pairing_t *pairing = malloc(sizeof(pairing_t));

  FILE * file = fopen(params_path, "r");
  size_t read = fread(param, 1, 1024, file);
  fclose(file);

  if (!read) pbc_die("Error reading the parameter file");

  pairing_init_set_buf(*pairing, param, read);

  return pairing;
}

//static element_t ** create_canonical_base

setup_t setup(const pairing_t* pairing, const int l) {
  //  element_t ** c_base = create_canonical_base(pairing, 3);
}

ciphertext_t encrypt(const pairing_t* pairing, const mpk_t public, const int x[], element_t *m) {

}

key_t keygen(const pairing_t* pairing, const msk_t private, const int y[]) {

}

element_t * decript(const pairing_t* pairing, const ciphertext_t ct, const key_t key) {

}


