#include "lostw.h"

element_t ** sampleRandomOrtonormalBases(const lostw_general_params *params, const element_t N) {
  return NULL;
}

lostw_setup_param * lostw_setup(unsigned int n) {
  pbc_param_t ec_params;
  unsigned int rbits = 180, qbits = 603;

  lostw_setup_param * params = malloc(sizeof(lostw_setup_param));
  lostw_general_params * genparams = params->params = malloc(sizeof(lostw_general_params));
  lostw_mpk * public = params->public = malloc(sizeof(lostw_mpk));
  lostw_msk * secret = params->secret = malloc(sizeof(lostw_msk));

  pbc_param_init_a_gen(ec_params, rbits, qbits);
  pairing_init_pbc_param(genparams->pairing, ec_params);
  
  genparams->n = n;
  element_init_G1(genparams->g, genparams->pairing);
  element_random(genparams->g);

  unsigned int N = 2 * n + 3;

  element_init_GT(public->sigma, genparams->pairing);
  pairing_apply(public->sigma, genparams->g, genparams->g, genparams->pairing);
  
  element_t ** dualOrthonormalBases = sampleRandomOrtonormalBases(genparams, N);
  element_t B[1];

  pbc_param_clear(ec_params);

  return params;
}
