#include <pbc/pbc.h>
#include <sys/time.h>

int main(int argc, char ** argv) {
  pbc_param_t ec_params;
  pairing_t pairing;
  unsigned int rbits = 40, qbits = 128;
  element_t *g1, *g2, *gt;
  
  for (int s = 10; s <= 1000; s += 10) {
  while (qbits <= 4096) {
    fprintf(stderr, "%d", qbits);
    pbc_param_init_a_gen(ec_params, rbits, qbits);

    pairing_init_pbc_param(pairing, ec_params);


    element_init_G1(g1, pairing);
    element_init_G2(g2, pairing);
    element_init_GT(gt, pairing);

    struct timeval tv1, tv2;
    int bc = element_length_in_bytes(g1);

    for (int i = 0; i < 100; i++) {
      if (0 == i % 10)
	fprintf(stderr, ".");

      element_random(g1);
      element_random(g2);

      gettimeofday(&tv1, NULL);
      pairing_apply(gt, g1, g2, pairing);
      gettimeofday(&tv2, NULL);

      double time = tv2.tv_sec - tv1.tv_sec;
      time *= (1000 * 1000);
      time += tv2.tv_usec - tv1.tv_usec;

      fprintf(stdout, "%d %d %d %d %d\n", bc, rbits, qbits, i, ((int) time));
    }

    for (int j = 0; j < s; j++) {
      element_clear(g1[j]);
      element_clear(g2[j]);
      element_clear(gt[j]);
    }

    rbits *= 2;
    qbits *= 2;

    fprintf(stderr, "\n");
  }
  free(g1);
  free(g2);
  free(g3);
  }
}
