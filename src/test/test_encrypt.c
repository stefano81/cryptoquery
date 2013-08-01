#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>
#include <open

#include <pbc/pbc.h>
#include "../schemas/utils.h"

#include <openssl/aes.h>

int main(int argc, char **argv) {
  pairing_t * pairing = load_pairing(argv[1]);
  
  element_t m;


  element_init_G1(m, *pairing);
  element_random(m);

  return 0;
}
