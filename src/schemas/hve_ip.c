#include "hve_ip.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>

#ifdef DEBUG
void print_vector(element_t *v, int N) {
  for (int i = 0; i < N; ++i) {
    element_fprintf(stderr, " %B", v[i]);
  }
  fprintf(stderr, "\n");
}

void print_matrix(element_t **m, int N) {
  for (int i = 0; i < N; ++i)
    print_vector(m[i], N);
}

void print_10_vector(element_t *v, int N) {
  for (int i = 0; i < N; ++i) {
    fprintf(stderr, " %s", element_is1(v[i]) ? "1" : element_is0(v[i]) ? "0" : "x");
  }
  fprintf(stderr, "\n");
}

void print_10_matrix(element_t **m, int N) {
  for (int i = 0; i < N; ++i)
    print_10_vector(m[i], N);
}
#endif

static element_t ** create_identity_matrix(pairing_t *pairing, int n) {
  element_t **I = malloc(sizeof(element_t *) * n);

  for (int i = 0; i < n; ++i) {
    I[i] = malloc(sizeof(element_t) * n);
    for (int j = 0; j < n; ++j) {
      element_init_Zr(I[i][j], *pairing);
      if (i == j)
	element_set1(I[i][j]);
      else
	element_set0(I[i][j]);
    }
#ifdef DEBUG
    fprintf(stderr, "IDENTITY ROW: %d\n", i);
    print_10_vector(I[i], n);
#endif
  }

#ifdef DEBUG
  fprintf(stderr, "IDENTITY:\n");
  print_10_matrix(I, n);
#endif

  return I;
}

static element_t ** sample_linear_transformation_Zr(pairing_t * pairing, int n) {
  element_t ** lt = malloc(sizeof(element_t *) * n);

  for (int i = 0; i < n; ++i) {
    lt[i] = malloc(sizeof(element_t) * n);
    for (int j = 0; j < n; ++j) {
      element_init_Zr(lt[i][j], *pairing);
      element_random(lt[i][j]);
    }
  }

  return lt;
}

static void copy(element_t **dst, element_t **src, int sizeY, int sizeX, int x, int y) {
  for (int i = y; i < sizeY; ++i) {
    for (int j = x; j < sizeX; ++j) {
      element_set(dst[i - y][j - x], src[i][j]);
    }
  }
}

static element_t ** invert2(element_t **m, int n) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (i == j)
	element_set1(m[i][j + n]);
      else
	element_set0(m[i][j + n]);
    }
  }

  element_t alpha, beta, t;
  element_init_same_as(alpha, m[0][0]);
  element_init_same_as(beta, alpha);
  element_init_same_as(t, beta);

  for (int i = 0; i < n; ++i) {
    element_set(alpha, m[i][i]);

    if (element_is0(alpha))
      pbc_die("Singular matric, cannot invert");
    else {
      for (int j = 0; j < n * 2; ++j) {
	element_div(m[i][j], m[i][j], alpha);
      }

      for (int k = 0; k < n; ++k) {
	if (0 != k - i) {
	  element_set(beta, m[k][i]);
	  for (int j = 0; j < n * 2; ++j) {
	    element_mul(t, beta, m[i][j]);
	    element_sub(m[k][j], m[k][j], t);
	  }
	}
      }
    }
  }

  element_clear(alpha);
  element_clear(beta);
  element_clear(t);

  return m;
}

static element_t ** invert(element_t **m, int n) {
  element_t **tm = malloc(sizeof(element_t*) * n);
#ifdef DEBUG
  element_t **rm = malloc(sizeof(element_t*) * n);
#endif
  
  for (int i = 0; i < n; ++i) {
#ifdef DEBUG
    rm[i] = malloc(sizeof(element_t));
    for (int j = 0; j < n; ++j)
      element_init_same_as(rm[i][j], m[0][0]);
#endif

    tm[i] = malloc(sizeof(element_t)*  2 * n);
    for (int j = 0; j < n * 2; ++j) {
      element_init_same_as(tm[i][j], m[0][0]);
    }
  }

  copy(tm, m, n, n, 0, 0);
  invert2(tm, n);
#ifdef DEBUG
  copy(rm, tm, n, 2 * n, 0, n);
#else
  copy(m, tm, n, 2 * n, 0, n);
#endif
  
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n * 2; ++j) {
      element_clear(tm[i][j]);
    }
    free(tm[i]);
  }
  
  free(tm);

#ifdef DEBUG
  return rm;
#else
  return m;
#endif
}

/*
static element_t * determinant(element_t **a, int n) {
   element_t * det = malloc(sizeof(element_t));
   element_t **m = NULL, t;

   element_init_same_as(*det, a[0][0]);
   element_init_same_as(t, a[0][0]);

   if (n == 1) { // Shouldn't get used
     element_set(*det, a[0][0]);
   } else if (n == 2) {
     element_mul(*det, a[0][0], a[1][1]);
     element_mul(t, a[1][0], a[0][1]);
     element_sub(*det, *det, t);
   } else {
     element_set0(*det);

      for (int j1 = 0; j1<n; j1++) {
	m = malloc((n-1)*sizeof(element_t *));
	for (int i = 0;i < n-1;i++) {
	  m[i] = malloc((n-1)*sizeof(element_t));
	}
	for (int i=1;i<n;i++) {
	  int j2 = 0;
	  for (int j=0;j<n;j++) {
	    if (j == j1)
	      continue;
	    element_init_same_as(m[i-1][j2], a[i][j]);
	    element_set(m[i-1][j2], a[i][j]);
	    ++j2;
	  }
	}
	element_t * d1 = determinant(m,n-1);
	element_mul(t, a[0][j1], *d1);
	element_clear(*d1);
	free(d1);
	if (!signbit(pow(-1.0,j1+2.0)))
	  element_neg(t, t);

	element_add(*det, *det, t);
	for (int i=0; i < n-1; ++i) {
	  for (int j = 0; j < n-1; ++j)
	    element_clear(m[i][j]);
	  free(m[i]);
	}
	free(m);
      }
   }

   element_clear(t);

   return det;
}

static element_t ** cofactor_matrix(element_t **a, int n) {
  element_t * det;
  element_t **c;
  element_t **b = malloc(sizeof(element_t*) * n);

  for (int i = 0; i < n; ++i) {
    b[i] = malloc(sizeof(element_t) * n);
    for (int j = 0; j < n; ++j)
      element_init_same_as(b[i][j], a[0][0]);
  }

  c = malloc((n - 1) * sizeof(element_t *));
  for (int i = 0; i < n - 1; ++i)
    c[i] = malloc((n-1)*sizeof(element_t));

  for (int j=0;j<n;j++) {
    for (int i=0;i<n;i++) {

      // Form the adjoint a_ij
      int i1 = 0;
      for (int ii = 0; ii < n; ++ii) {
	if (ii == i)
	  continue;
	int j1 = 0;
	for (int jj = 0; jj < n; ++jj) {
	  if (jj == j)
	    continue;
	  element_init_same_as(c[i1][j1], a[ii][jj]);
	  element_set(c[i1][j1], a[ii][jj]);
	  j1++;
	}
	i1++;
      }

      // Calculate the determinate
      det = determinant(c, n-1);

      // Fill in the elements of the cofactor
      if (!pow(-1.0,i+j+2.0))
	element_neg(b[i][j], *det);
      else
	element_set(b[i][j], *det);

      element_clear(*det);
      free(det);
    }
  }

  for (int i = 0; i < n - 1; ++i) {
    for (int j = 0; j < n -1; ++j) {
      element_clear(c[i][j]);
      element_clear(a[i][j]);
    }
    free(a[i]);
    free(c[i]);
  }
  free(a);
  free(c);

  return b;
}

static element_t ** invert_square(element_t **m, int n) {
  element_t * det = determinant(m, n);

  element_t ** m1 = cofactor_matrix(m, n);

  m1 = transpose(m1, n);

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      element_div(m1[i][j], m1[i][j], det);

  return m1;
}
*/

setup_t setup(pairing_t *pairing, int l) {
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
        //element_set0(tmp1); element_set0(tmp2);
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

/* setup_t setup(pairing_t* pairing, int l) { */
/*   element_t **cbase, psi, t; */

/*   setup_t out = malloc(sizeof(*out)); */
/*   msk_t private = out->private = malloc(sizeof(*private)); */
/*   mpk_t public = out->public = malloc(sizeof(*public)); */

/*   cbase = create_identity_matrix(pairing, 3); */

/*   element_init_Zr(psi, *pairing); */
/*   element_random(psi); */

/* #ifdef DEBUG */
/*   //element_fprintf(stderr, "psi: %B\n", psi); */
/* #endif */

/*   for (int i = 0; i < 3; ++i) */
/*     //element_mul_zn(cbase[i][i], cbase[i][i], psi); // now \psi-identity -> canonical base */
/*     element_mul(cbase[i][i], cbase[i][i], psi); */

/* #ifdef DEBUG */
/*   //print_10_matrix(cbase, 3); */
/* #endif */

/*   element_init_G1(out->g, *pairing); */
/*   element_random(out->g); */

/*   private->l = public->l = l; */
/*   private->C = malloc(sizeof(element_t **) * (l + 1)); */
/*   public->B = malloc(sizeof(element_t **) * (l + 1)); */
  

/*   element_init_GT(t, *pairing); */
/*   element_init_GT(public->gT, *pairing); */

/*   pairing_apply(t, out->g, out->g, *pairing); */
/*   element_pow_zn(public->gT, t, psi); */

/*   element_clear(t); */

/*   element_init_Zr(t, *pairing); */

/*   for (int k = 0; k <= l; ++k) { */
/*     element_t ** lt = sample_linear_transformation_Zr(pairing, 3); */

/*     element_t ** B = public->B[k] = malloc(sizeof(element_t *) * 3); */

/*     for (int i = 0; i < 3; ++i) { */
/*       B[i] = malloc(sizeof(element_t) * 3); */

/*       for (int j = 0; j < 3; ++j) { */
/* 	element_init_Zr(B[i][j], *pairing); */
/* 	element_mul(B[i][j], cbase[0][j], lt[i][0]); */
/*       } */
/*       for (int r = 1; r < 3; ++r) { */
/* 	for (int j = 0; j < 3; ++j) { */
/* 	  element_mul(t, cbase[r][j], lt[i][r]); */
/* 	  element_add(B[i][j], B[i][j], t); */
/* 	} */
/*       } */
/*     } */

/* #ifdef DEBUG */
/*     //fprintf(stderr, "B %d\n", k); */
/*     //print_matrix(B, 3); */
/* #endif */

/* #ifdef DEBUG */
/*     element_t ** oldlt = lt; */
/*     lt = invert(transpose(lt, 3), 3); */
/*     //lt = invert_square(lt, 3); //traspose(lt, 3), 3); */
/* #else */
/*     lt = invert(transpose(lt, 3), 3); */
/* #endif */

/* #ifdef DEBUG */
/*     //fprintf(stderr, "LT^{-1} %d\n", k); */
/*     //print_matrix(lt, 3); */
/* #endif */

/*     element_t ** C = private->C[k] = malloc(sizeof(element_t*) * 3); */

/*     for (int i = 0; i < 3; ++i) { */
/*       C[i] = malloc(sizeof(element_t) * 3); */

/*       for (int j = 0; j < 3; ++j) { */
/* 	element_init_Zr(C[i][j], *pairing); */
/* 	element_mul(C[i][j], cbase[0][j], lt[i][0]); */
/*       } */
/*       for (int r = 1; r < 3; ++r) { */
/* 	for (int j = 0; j < 3; ++j) { */
/* 	  element_mul(t, cbase[r][j], lt[i][r]); */
/* 	  element_add(C[i][j], C[i][j], t); */
/* 	} */
/*       } */
/*     } */

/*     for (int i = 0; i < 3; ++i) { */
/*       for (int j = 0; j < 3; ++j) */
/* #ifdef DEBUG */
/* 	{ */
/* 	element_clear(oldlt[i][j]); */
/* #endif */
/*        element_clear(lt[i][j]); */
/* #ifdef DEBUG */
/* 	} */
/*       free(oldlt[i]); */
/* #endif */
/*       free(lt[i]); */

/*     } */
/* #ifdef DEBUG */
/*     free(oldlt); */
/* #endif */
/*     free(lt); */
/*   } */

/*   for (int i = 0; i < 3; ++i) { */
/*     for (int j = 0; j < 3; ++j) */
/*       element_clear(cbase[i][j]); */

/*     free(cbase[i]); */
/*   } */
/*   free(cbase); */
/*   element_clear(psi); */
/*   element_clear(t); */

/*   return out; */
/* } */

ciphertext_t encrypt(pairing_t* pairing, mpk_t public, int x[], element_t *m) {
  ciphertext_t ct = malloc(sizeof(*ct));
  element_t one, xt, v[3],
    z,
    wt;

  element_init_GT(*m, *pairing);
  element_random(*m);

#ifdef DEBUG
  element_t * w = ct->w = malloc(sizeof(element_t) * (public->l + 1));
  ct->x = malloc(sizeof(int) * public->l);
  element_init_same_as(ct->gT, public->gT);
  element_set(ct->gT, public->gT);
#endif

  ct->l = public->l;

  element_init_Zr(xt, *pairing);

  //element_init_G1(one, *pairing);
  //element_set1(one);

  ct->ci = malloc(sizeof(element_t) * (ct->l + 1));

  element_init_Zr(z, *pairing);
  element_init_Zr(wt, *pairing);
  element_random(z);

#ifdef DEBUG
  element_init_same_as(ct->z, z);
  element_set(ct->z, z);
#endif

  element_init_GT(ct->c, *pairing);
  element_pow_zn(ct->c, public->gT, z);
  element_mul(ct->c, ct->c, *m); // c = g_T ^ z \times m

  element_init_Zr(v[0], *pairing);
  element_init_Zr(v[1], *pairing);
  element_init_Zr(v[2], *pairing);

  element_random(wt); // w_0

#ifdef DEBUG
  element_init_same_as(w[0], wt);
  element_set(w[0], wt);
#endif

  /*element_mul(v[0], one, wt);
  element_mul(v[1], one, z);
  */
  element_set(v[0], wt);
  element_set(v[1], z);
  element_set0(v[2]); // (w_0, z, 0)

  ct->ci[0] = vector_times_matrix(pairing, v, public->B[0], 3);

  //element_mul_zn(v[2], one, wt); // set w0 to the third element of the vector
  element_set(v[2], wt); // set w0 to the third element of the vector

  for (int i = 1; i <= ct->l; ++i) {
    element_random(wt); // wt <- F_q

#ifdef DEBUG
    element_init_same_as(w[i], wt);
    element_set(w[i], wt);
    ct->x[i - 1] = x[i - 1];
#endif

    //element_mul_zn(v[0], one, wt);
    element_set(v[0], wt);
    element_set_si(xt, x[i-1]);
    element_mul(wt, wt, xt);
    //element_mul_zn(v[1], one, wt);
    element_set(v[1], wt); // (w_t, w_t x_t, w_0)

    ct->ci[i]  = vector_times_matrix(pairing, v, public->B[i], 3);
  }

  element_clear(v[0]);
  element_clear(v[1]);
  element_clear(v[2]);
  element_clear(wt);
  element_clear(xt);
  element_clear(z);

  return ct;
}

dkey_t keygen(pairing_t* pairing, msk_t private, int y[]) {
  dkey_t k = malloc(sizeof(*k));
  element_t yt, dt, st, s0, v[3], one;

  k->S = 0;
  k->k = malloc(sizeof(element_t) * (private->l + 1));

#ifdef DEBUG
  element_t *s = k->s = malloc(sizeof(element_t) * (private->l + 1));
  element_t *d = k->d = malloc(sizeof(element_t) * (private->l + 1));
  k->y = malloc(sizeof(int) * private->l);
#endif

  element_init_G1(one, *pairing);
  element_init_Zr(v[0], *pairing);
  element_init_Zr(v[1], *pairing);
  element_init_Zr(v[2], *pairing);
  element_init_Zr(dt, *pairing);
  element_init_Zr(yt, *pairing);
  element_init_Zr(s0, *pairing);
  element_init_Zr(st, *pairing);

  element_set1(one);
  element_set0(s0);

  unsigned long mask = 0x01;
  for (int i = 1; i <= private->l; ++i, mask <<= 1) {
#ifdef DEBUG
    k->y[i - 1] = y[i - 1];
#endif

    if (-1 == y[i - 1]) {
      k->k[i] = NULL;

      continue;
    }

    k->S |= mask;

    element_random(dt);
    element_random(st);
    element_add(s0, s0, st); // sum st

#ifdef DEBUG
    element_init_same_as(d[i], dt);
    element_set(d[i], dt);
    element_init_same_as(s[i], st);
    element_set(s[i], st);
#endif

    element_set_si(yt, y[i - 1]);
    //element_mul(yt, dt, yt);
    element_mul(v[0], dt, yt);
    //element_neg(dt, dt);
    //element_mul_zn(v[1], one, dt);
    element_neg(v[1], dt);
    //element_mul_zn(v[2], one, st);
    element_set(v[2], st); // (d_t \times y_t, -d_t, s_t)

    k->k[i] = vector_times_matrix(pairing, v, private->C[i], 3);
  }

  element_neg(s0, s0); // s0 = -(\sum_{t \in S} s_t)

  element_mul_zn(v[0], one, s0);
  element_set1(v[1]);
  element_random(v[2]); // (s_0, 1, eta)

#ifdef DEBUG
  element_init_same_as(k->s[0], s0);
  element_set(k->s[0], s0);
  element_init_same_as(k->eta, v[2]);
  element_set(k->eta, v[2]);
#endif

  k->k[0] = vector_times_matrix(pairing, v, private->C[0], 3);

  element_clear(v[0]);
  element_clear(v[1]);
  element_clear(v[2]);
  element_clear(dt);
  element_clear(st);
  element_clear(yt);
  element_clear(s0);
  element_clear(one);

  return k;
}

#ifdef DEBUG
int MYHVE(int *x, int *y, int n) {
  for (int i = 0; i < n; ++i)
    if (-1 != y[i] && x[i] != y[i])
      return 0;

  return 1;
}
#endif

element_t * decrypt(pairing_t* pairing, ciphertext_t ct, dkey_t key) {
  element_t * m = malloc(sizeof(element_t));
  element_t t1, t2;

#ifdef DEBUG
  element_t _t1, _t2;

  fprintf(stderr, "HVE(X, Y) = %d\n", MYHVE(ct->x, key->y, ct->l));
#endif

  element_init_GT(*m, *pairing);
  element_init_GT(t1, *pairing);
  element_init_GT(t2, *pairing);

#ifdef DEBUG
  element_init_same_as(_t1, t2);
  element_init_same_as(_t2, t2);
  
  pairing_apply(t2, key->k[0][0], ct->ci[0][0], *pairing);

  element_pow_zn(_t1, ct->gT, key->s[0]);
  element_pow_zn(_t1, _t1, ct->w[0]);

  fprintf(stderr, "e(k0', c0') == gT^{s0w0}? %s\n", 0 == element_cmp(t2, _t1) ? "true" : "false");
  fprintf(stderr, "e(k0', c0') == 1? %s\n", element_is1(t2) ? "true" : "false");
  fprintf(stderr, "e(k0', c0') == 0? %s\n", element_is0(t2) ? "true" : "false");

  pairing_apply(_t2, key->k[0][1], ct->ci[0][1], *pairing);
  element_mul(t2, t2, _t2);

  element_pow_zn(_t1, ct->gT, ct->z);

  fprintf(stderr, "e(k0'', c0'') == gT^{z}? %s\n", 0 == element_cmp(_t2, _t1) ? "true" : "false");
  fprintf(stderr, "e(k0'', c0'') == 1? %s\n", element_is1(_t2) ? "true" : "false");
  fprintf(stderr, "e(k0'', c0'') == 0? %s\n", element_is0(_t2) ? "true" : "false");

  pairing_apply(_t2, key->k[0][2], ct->ci[0][2], *pairing);
  element_mul(t2, t2, _t2);

  fprintf(stderr, "e(k0''', c0''') == 1? %s\n", element_is1(_t2) ? "true" : "false");
  fprintf(stderr, "e(k0''', c0''') == 0? %s\n", element_is0(_t2) ? "true" : "false");

#else
  element_prod_pairing(t2, key->k[0], ct->ci[0], 3); // p_0 = e(k_0, c_0)
#endif

  unsigned long mask = 0x01;
  for (int i = 1; i <= ct->l; ++i, mask <<= 1) {
    if (key->S & mask) {
      // i \in S
      element_prod_pairing(t1, key->k[i], ct->ci[i], 3); // p_t =e(k_t, c_t)
      element_mul(t2, t2, t1); // den = p_0 \times \Pi_{t \in S} p_t
    }
  }

  element_div(*m, ct->c, t2); // c / den

  element_clear(t1);
  element_clear(t2);

#ifdef DEBUG
  element_clear(_t1);
  element_clear(_t2);
#endif

  return m;
}


int serialize_ct(unsigned char ** buffer, ciphertext_t ct) {
  int size = sizeof(unsigned short) + element_length_in_bytes(ct->c) + (ct->l * element_length_in_bytes(ct->ci[0][0]));
  unsigned char * tbuf, * buff = tbuf = *buffer = malloc(size);
  unsigned short l1 = htons(ct->l);
  memcpy(buff, &(l1), sizeof(unsigned short));
  tbuf += sizeof(unsigned short);

  tbuf += element_to_bytes(tbuf, ct->c);

  for (int i = 0; i <= ct->l; ++i)
    for (int j = 0; j < 3; ++j)
      tbuf += element_to_bytes(tbuf, ct->ci[i][j]);

  return size;
}

ciphertext_t deserialize_ct(unsigned char *buffer) {
  ciphertext_t ct = malloc(sizeof(*ct));

  unsigned short l1;
  memcpy(&l1, buffer, sizeof(unsigned short));
  ct->l = ntohs(l1);

  buffer += sizeof(unsigned short);

  buffer += element_from_bytes(ct->c, buffer);
  ct->ci = malloc(sizeof(element_t *) * ct->l);
  for (int i = 0; i <= ct->l; ++i) {
    ct->ci[i] = malloc(sizeof(element_t) * 3);
    for (int j = 0; j < 3; ++j)
      buffer += element_from_bytes(ct->ci[i][j], buffer);
  }

  return ct;
}

int serialize_mpk(unsigned char ** buffer, mpk_t pulbic) {
}

mpk_t deserialize_mpk(unsigned char * buffer) {
}

int serialize_msk(unsigned char ** buffer, msk_t private) {
}

msk_t deserialize_msk(unsigned char * buffer) {
}

int serialize_key(unsigned char ** buffer, dkey_t k) {
}

dkey_t deserialize_key(unsigned char * buffer) {
}
