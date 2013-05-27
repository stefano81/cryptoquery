#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/aes.h>
#include "schemas/utils.h"

#include "schemas/hve_ip.h"

void minorX(element_t * tmp1, element_t a, element_t b, element_t c, element_t d){
  element_t tmp2;

  element_init_same_as(*tmp1,a);
  element_init_same_as(tmp2,a);
  element_mul(*tmp1,a,b);
  element_mul(tmp2,c,d);
  element_sub(*tmp1,*tmp1,tmp2);

  element_clear(tmp2);
}
    

setup_t setupAlgo(pairing_t *pairing, int l) {
  element_t ***BB, ***CC;
  element_t g1,g2,psi,**A1, **A2, **B, **C, **X, **Xs;
  element_t tmp1,tmp2;
  element_t ptmp;
  element_t t,gT;
  element_t t1,t2,t3;
  element_t delta;
  
  element_init_Zr(delta,*pairing); 
  element_init_Zr(t1,*pairing);
  element_init_Zr(t2,*pairing);
  element_init_Zr(t3,*pairing);
  

  /* pick random psi in the field */
  element_init_Zr(psi, *pairing);
  element_random(psi);

  /* any element of G1/G2 other than 0 is a generator*/
  element_init_G1(g1, *pairing);
  element_random(g1);
  element_init_G2(g2, *pairing);
  element_random(g2);

  /* compute gT=e(g1,g2)^psi */
  element_init_GT(gT, *pairing);
  element_init_GT(t, *pairing);
  pairing_apply(t, g1, g2, *pairing);
  element_pow_zn(gT, t, psi);

  element_init_G1(tmp1,*pairing);
  element_init_G2(tmp2,*pairing);


  /* if you want to see psi, g1, g2, gT */
#ifdef DEBUG
  printf("psi=\n");
  element_printf("%B\n",psi);  
  printf("g1=\n");
  element_printf("%B\n",g1);  
  printf("g2=\n");
  element_printf("%B\n",g2);  
  printf("gT=\n");
  element_printf("%B\n",gT);  
#endif

  /* constructing the canonical bases: A1 for G1 and A2 for G2 */
  A1=malloc(sizeof(element_t *)*3);  A1[0]=malloc(sizeof(element_t)*3); 
  A1[1]=malloc(sizeof(element_t)*3); A1[2]=malloc(sizeof(element_t)*3); 

  A2=malloc(sizeof(element_t *)*3);  A2[0]=malloc(sizeof(element_t)*3); 
  A2[1]=malloc(sizeof(element_t)*3); A2[2]=malloc(sizeof(element_t)*3); 

  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++){
      element_init_G1(A1[i][j],*pairing); element_init_G2(A2[i][j],*pairing);
      if (i==j){
	element_set(A1[i][j],g1);  /* element_set(dest,source) */
	element_set(A2[i][j],g2);  /* element_set(dest,source) */
      }
      else{
	element_set0(A1[i][j]);
	element_set0(A2[i][j]);
      }
    }
  }

  BB=(element_t ***)malloc(sizeof(element_t **)*(l+1));
  CC=(element_t ***)malloc(sizeof(element_t **)*(l+1));

  setup_t setup = malloc(sizeof(setup_t));

  /* qui ho fatto un qualche errore con i puntatori */
  mpk_t public = setup->public = malloc(sizeof(*public));
  msk_t private = setup->private = malloc(sizeof(*private));
  element_init_GT(public->gT,*pairing);
  element_set(public->gT, gT);
  /*  element_init_G1(public->g1, *pairing);
      element_set(public->g1, g1);
      element_init_G2(setup->public->g2,*pairing);
      element_set(public->g2, g2);*/
  setup->private->l = setup->public->l = l;
  setup->public->B = BB;
  setup->private->C = CC;


  for(int zz=0;zz<l+1;zz++){
    BB[zz]=malloc(sizeof(element_t *)*3); CC[zz]=malloc(sizeof(element_t *)*3);
    BB[zz][0]=malloc(sizeof(element_t)*3); CC[zz][0]=malloc(sizeof(element_t)*3);
    BB[zz][1]=malloc(sizeof(element_t)*3); CC[zz][1]=malloc(sizeof(element_t)*3);
    BB[zz][2]=malloc(sizeof(element_t)*3); CC[zz][2]=malloc(sizeof(element_t)*3);

    B=BB[zz]; C=CC[zz];
    X=malloc(sizeof(element_t *)*3); Xs=malloc(sizeof(element_t *)*3);
    X[0]=malloc(sizeof(element_t)*3); Xs[0]=malloc(sizeof(element_t)*3);
    X[1]=malloc(sizeof(element_t)*3); Xs[1]=malloc(sizeof(element_t)*3);
    X[2]=malloc(sizeof(element_t)*3); Xs[2]=malloc(sizeof(element_t)*3);
    /* pick a random X */
    for(int i=0;i<3;i++){
      for(int j=0;j<3;j++){
        element_init_Zr(X[i][j],*pairing);
	element_random(X[i][j]);
      }
    }

    /* let us invert X in Xs*/
    minorX(&ptmp, X[1][1],X[2][2],X[1][2],X[2][1]);
    element_init_Zr(Xs[0][0],*pairing); 
    element_set(Xs[0][0],ptmp); element_clear(ptmp);
    minorX(&ptmp, X[1][2],X[2][0],X[1][0],X[2][2]);
    element_init_Zr(Xs[1][0],*pairing); 
    element_set(Xs[1][0],ptmp); element_clear(ptmp);
    minorX(&ptmp, X[1][0],X[2][1],X[1][1],X[2][0]);
    element_init_Zr(Xs[2][0],*pairing); 
    element_set(Xs[2][0],ptmp); element_clear(ptmp);

    minorX(&ptmp, X[0][2],X[2][1],X[0][1],X[2][2]);
    element_init_Zr(Xs[0][1],*pairing); 
    element_set(Xs[0][1],ptmp); element_clear(ptmp);
    minorX(&ptmp, X[0][0],X[2][2],X[0][2],X[2][0]);
    element_init_Zr(Xs[1][1],*pairing); 
    element_set(Xs[1][1],ptmp); element_clear(ptmp);
    minorX(&ptmp, X[0][1],X[2][0],X[0][0],X[2][1]);
    element_init_Zr(Xs[2][1],*pairing); 
    element_set(Xs[2][1],ptmp); element_clear(ptmp);

    minorX(&ptmp, X[0][1],X[1][2],X[0][2],X[1][1]);
    element_init_Zr(Xs[0][2],*pairing); 
    element_set(Xs[0][2],ptmp); element_clear(ptmp);
    minorX(&ptmp, X[0][2],X[1][0],X[0][0],X[1][2]);
    element_init_Zr(Xs[1][2],*pairing); 
    element_set(Xs[1][2],ptmp); element_clear(ptmp);
    minorX(&ptmp, X[0][0],X[1][1],X[0][1],X[1][0]);
    element_init_Zr(Xs[2][2],*pairing); 
    element_set(Xs[2][2],ptmp); element_clear(ptmp);

    /* computing the determinat of X */
    element_mul(t1,X[0][0],X[1][1]); element_mul(t1,t1,X[2][2]);
    element_mul(t2,X[0][1],X[1][2]); element_mul(t2,t2,X[2][0]);
    element_mul(t3,X[0][2],X[1][0]); element_mul(t3,t3,X[2][1]);
    element_add(delta,t1,t2); element_add(delta,delta,t3);
    element_mul(t1,X[0][2],X[1][1]); element_mul(t1,t1,X[2][0]);
    element_mul(t2,X[0][0],X[1][2]); element_mul(t2,t2,X[2][1]);
    element_mul(t3,X[0][1],X[1][0]); element_mul(t3,t3,X[2][2]);
    element_sub(delta,delta,t1);   element_sub(delta,delta,t2);
    element_sub(delta,delta,t3);  

    for(int i=0;i<3;i++){
      for(int j=0;j<3;j++){
        element_div(Xs[i][j],Xs[i][j],delta);
        element_mul(Xs[i][j],Xs[i][j],psi);
      }
    }
    /* now X * Xs = psi * I */
  
    /* if you want to check X and Xs */
#ifdef DEBUG
    for(int i=0; i<3; ++i){
      for(int j=0; j<3; ++j){
	printf("(%d %d)\n", i, j);
        element_mul(t1,X[i][0],Xs[0][j]);
        element_mul(t2,X[i][1],Xs[1][j]);
        element_mul(t3,X[i][2],Xs[2][j]);
        element_add(t1,t1,t2);
        element_add(t1,t1,t3);
        element_printf("(X*Xs)[i][j] %B\n",t1);
      }
    }
#endif

    /* now we compute B=X x A1 and C=Xs x A2 */
    for(int i=0;i<3;i++){
      for(int j=0;j<3;j++){
        element_init_G1(B[i][j],*pairing);
        element_init_G2(C[i][j],*pairing);
        element_set0(tmp1); element_set0(tmp2);
        for(int k=0;k<3;k++){
	  element_mul_zn(tmp1,A1[k][j],X[i][k]);
	  element_mul_zn(tmp2,A2[k][j],Xs[i][k]);
	  element_add(B[i][j],B[i][j],tmp1);
	  element_add(C[i][j],C[i][j],tmp2);
        }
      }
    }

    C = transpose(C, 3);

    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
	element_clear(X[i][j]);
	element_clear(Xs[i][j]);
      }
      free(X[i]);
      free(Xs[i]);
    }
    free(X);
    free(Xs);

  } /* for zz */

#ifdef DEBUG
  element_t check, oneT;
  element_init_GT(check, *pairing);
  element_init_GT(oneT, *pairing);
  element_set1(oneT);
  for (int k = 0; k < l + 1; ++k) {
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
	element_prod_pairing(check, BB[k][i], CC[k][j], 3);
	if (i == j)
	  fprintf(stderr, "[%d] %d == %d -> gT: %s\n", k, i, j, 0 == element_cmp(gT, check) ? "true" : "false");
	else
	  fprintf(stderr, "[%d] %d != %d -> 1T: %s\n", k, i, j, 0 == element_cmp(oneT, check) ? "true" : "false");
      }
    }
  }
  element_clear(oneT);
  element_clear(check);
#endif
  element_clear(tmp1);
  element_clear(tmp2);
  element_clear(delta); 
  element_clear(t1);
  element_clear(t2);
  element_clear(t3);

  element_clear(g1);
  element_clear(g2);
  element_clear(psi);
  element_clear(t);
  element_clear(gT);

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      element_clear(A1[i][j]);
      element_clear(A2[i][j]);
    }
    free(A1[i]);
    free(A2[i]);
  }
  free(A1);
  free(A2);

  return setup;
}


void test_fixed(const char *path) {
  pairing_t * pairing = load_pairing(path);

  int x[] = {1, 2, 3, 4, 5};
  int y1[] = {1, 2, 3, 4, 5};
  int y2[] = {-1, -1, 3, 4, 5};
  int y3[] = {2, 3, 4, 5, 6};

  //setup_t out = setup(pairing, 5);
  setup_t out = setupAlgo(pairing, 5);
  element_t m, *dm;

  ciphertext_t ct = encrypt(pairing, out->public, x, &m);

  dkey_t key1 = keygen(pairing, out->private, y1);
  dm = decrypt(pairing, ct, key1);
  int r = element_cmp(m, *dm);
  printf("1: %s\n", !r ? "OK!" : "No!");

  dkey_t key2 = keygen(pairing, out->private, y2);
  dm = decrypt(pairing, ct, key2);
  r = element_cmp(m, *dm);
  printf("2: %s\n", !r ? "OK!" : "No!");

  dkey_t key3 = keygen(pairing, out->private, y3);
  dm = decrypt(pairing, ct, key3);
  r = element_cmp(m, *dm);
  printf("3: %s\n", !r ? "OK!" : "No!");
}

int main(int argc, char ** argv) {
  if (2 == argc)
    test_fixed(argv[1]);
  /*   else if (4 == argc) */
  /*     test_variable(argv[1], atoi(argv[2]), atoi(argv[3])); */
  else
    printf("error testing");

  return 0;
}
