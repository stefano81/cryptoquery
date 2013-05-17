#include "hve.h"

setup_output * hve_setup(const unsigned int k, const unsigned int n, char path_name[]) {
  setup_output * output = malloc(sizeof(setup_output));

  char param[1024];
  FILE * file;

  return output;
}

ciphertext * hve_enc(const master_public_key *public, const int x, element_t *m) {
  return NULL;
}

key * hve_keygen(const master_private_key *secret, const int y) {
  return NULL;
}

/*element_t hve_decrypt(const ciphertext *ct, key *private) {
  
return NULL;
  }*/
