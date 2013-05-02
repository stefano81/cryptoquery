#include "ksw_predicate.h"

ksw_setup_t * ksw_setup(unsigned int n) {
  // init the parameters
  mpz_t op, oq,or, on, gp, gq, gr;
  gmp_randstate_t state;

  mpz_inits(on, op, oq, or, gp, gr, NULL);
  gmp_randinit_default(state);

  mpz_urandomb(op, state, 256);
  mpz_nextprime(op, op);

  mpz_urandomb(oq, state, 256);
  mpz_nextprime(oq, oq);

  mpz_urandomb(or, state, 256);
  mpz_nextprime(or, or);

  mpz_add(on, on, op);
  mpz_add(on, on, oq);
  mpz_add(on, on, or);

  ksw_setup_t * setup = malloc(sizeof(ksw_setup_t));
  ksw_mpk_t * public = setup->public = malloc(sizeof(ksw_mpk_t));
  ksw_mpk_t * secret = setup->secret = malloc(sizeof(ksw_msk_t));

  pbc_param_t param;

  pbc_param_init_a1_gen(param, on);
  pairing_init_pbc_param(setup->pairing, param);
  
  // secret
  element_init_Zr(secret->p, setup->pairing);
  element_set_mpz(p, op);

  element_init_Zr(secret->q, setup->pairing);
  element_set_mpz(q, oq);
  
  element_init_Zr(secret->r, setup->pairing);
  element_set_mpz(r, or);

  element_init_G1(secret->gq, setup->pairing);
  mpz_urandomb(gq, state, 256);
  mpz_mod(gq, gq, op);
  element_set_mpz(secret->gq, gq);
  

  // public
  element_init_G1(public->gp, setup->pairing);
  mpz_urandomb(gp, state, 256);
  mpz_mod(gp, gp, op);
  element_set_mpz(public->gp, gp);

  element_init_G1(public->gr, setup->pairing);
  mpz_urandomb(gr, state, 256);
  mpz_mod(gr, gr, or);
  element_set_mpz(public->gr, gr);

  // clean up
  pbc_param_clear(param);
  gmp_randclear(state);
  mpz_clears(on, op, oq, or, gp, gq, gr, NULL);

  return setup;
}

ksw_ct_t * ksw_enc(ksw_mpk_t * public, element_t * x, element_t * m) {

}

ksw_key_t * ksw_genkey(ksw_msk_t * secret, element_t * v) {

}

element_t * ksw_dec(ksw_key_t * key, ksw_ct_t * ct) {

}
