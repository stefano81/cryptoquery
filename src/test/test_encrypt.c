#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>

#include <gmp.h>
#include <pbc/pbc.h>
#include "../schemas/utils.h"

#include <openssl/aes.h>

int main(int argc, char **argv) {
  pairing_t * pairing = load_pairing(argv[1]);
  
  element_t m;


  element_init_G1(m, *pairing);
  element_random(m);

  element_printf("%B\n", m);
  mpz_t n;

  mpz_init(n);

  element_to_mpz(n, m);

  printf("%d %d %d %d %d %d\n",
	 mpz_fits_ulong_p(n),
	 mpz_fits_slong_p(n),
	 mpz_fits_uint_p(n),
	 mpz_fits_sint_p(n),
	 mpz_fits_ushort_p(n),
	 mpz_fits_sshort_p(n));

  return 0;
}
