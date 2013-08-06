#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>

#include <gmp.h>
#include <pbc/pbc.h>
#include "../schemas/utils.h"
#include "../schemas/aes.h"

#include <openssl/aes.h>

#include <assert.h>

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

  int x[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  unsigned char *in = "TESTSTRING", buff[AES_BLOCK_SIZE * 10], out[AES_BLOCK_SIZE * 10];
  unsigned char pwd[AES_BLOCK_SIZE];
  for (int i = 0; i < AES_BLOCK_SIZE; ++i)
    pwd[i] = '1';

  memcpy(buff, 0, AES_BLOCK_SIZE * 10);
  memcpy(out, 0, AES_BLOCK_SIZE * 10);

  AES_KEY ek, dk;

  AES_set_encrypt_key(pwd, 128, &ek);
  AES_set_decrypt_key(pwd, 128, &dk);

  unsigned char eIV[AES_BLOCK_SIZE] = {0}, dIV[AES_BLOCK_SIZE] = {0};

  AES_cbc_encrypt(in, buff, sizeof(in), &ek, eIV, AES_ENCRYPT);

  AES_cbc_encrypt(buff, out, sizeof(buff), &dk, dIV, AES_DECRYPT);

  //assert(strlen(in) == strlen(out));

  /*unsigned char *out = encrypt_AES(m, x, 10);

  int *y = decrypt_AES(m, out, 10);

  for (int i = 0; i < 10; ++i) {
    assert(x[i] == y[i]);
    }*/

  return 0;
}
