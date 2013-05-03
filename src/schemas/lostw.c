#include "lostw.h"

lostw_setup_param * lostw_setup(unsigned int n) {
  pbc_param_t params;
  unsigned int rbits = 180, qbits = 603;

  lostw_setup_param * params = malloc(sizeof(lostw_setup_param));
  lostw_general_params * genparams = params->params = malloc(sizeof(lostw_general_params));
  lostw_mpk * public = params->public = malloc(sizeof(lostw_mpk));
  lostw_msk * secret = params->secret = mallok(sizeof(lostw_msk));

  pbc_param_init_a_gen(params, 180, 603);
  pairing_init_pbc_param(genparams->pairing, params);
  
  genparams->n = n;
  element_init_G1(genparams->g, genparams->pairing);
  element_random(genparams->g);

  unsigned int N = 2 * n + 3;

  element_init_GT(public->sigma, genparams->pairing);
  pairing_apply(public->sigma, genparameters->g, genparameters->g);
  
  element_t dualOrthonormalBases[][] = sampleRandomOrtonormalBases(genparameters, N);
  element_t B[]

  pbc_param_clear(parmam);
}


int main(int argc, char * argv[]) {
  

  return 0;
}
