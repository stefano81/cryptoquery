#include <stdio.h>
#include <pbc/pbc.h>
#include "../schemas/hve_ip.h"
#include "../schemas/utils.h"

int main(int argc, char *argv[]) {
  pairing_t *pairing = load_pairing(argv[1]);

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
