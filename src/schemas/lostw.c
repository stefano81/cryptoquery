#include "lostw.h"

element_t ** sample_uniform_transformation(pairing_t pairing, int n) {
  int i, j;
  element_t ** transformation = malloc(sizeof(element_t *) * n);

  for (i = 0; i < n; i++) {
    transformation[i] = malloc(sizeof(element_t) * n);
    for (j = 0; j < n; j++) {
      element_init_Zr(transformation[i][j], pairing);
      element_random(transformation[i][j]);
    }
  }

  return transformation;
}

element_t ** traspose(element_t ** matrix, int n) {
  element_t t;
  int i, j;

  element_init_same_as(t, matrix[0][0]);

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      element_set(t, matrix[i][j]);
      element_set(matrix[i][j], matrix[j][i]);
      element_set(matrix[j][i], t);
    }
  }

  element_clear(t);

  return matrix;
}

void array_copy(element_t ** d, element_t ** s, int size_x, int size_y, int x, int y) {
  int i, j, k;

  for (i = y; i < size_y; i++) {
    for (j = x; j < size_x; j++) {
      element_set(d[i - y][j - x], s[i][j]);
    }
  }
}

element_t ** inv(element_t ** m, int n) {
  int i, j;
  int n2 = 2 * n;
  element_t a, b;

  element_init_same_as(a, m[0][0]);
  element_init_same_as(b, m[0][0]);

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      element_set0(m[i][j + n]);
    }
    element_set1(m[i][i+n]);
  }



  for (i = 0; i < n; i++) {
    element_set(a, m[i][j]);

    if (element_is0(a)) {
      fprintf(stderr, "IS ZERO!");
      return 1;
    } else {
      // normalize the vector
      for (j = 0; j < n2; j++) {
	element_div(m[i][j], m[i][j], a);
      }

      // subtract the vector from all other vectors to zero the
      // relevant matrix elements in the current column
      
    }
  }

  element_clear(t);

  return m;
}

element_t ** invert(element_t ** matrix, int n) {
  int i, j;
  element_t ** tmatrix = malloc(sizeof(element_t *) * n);

  for (i = 0; i < n; i++) {
    tmatrix[i] = malloc(sizeof(element_t) * (2 * n));
    for (j = 0; j < (2*n); j++) {
      element_init_same_as(tmatrix[i][j], matrix[0][0]);
    }
  }

  array_copy(tmatrix, matrix, n, n, 0, 0);

  array_copy(matrix, inv(tmatrix, n), n, 2 * n, 0, n);

  return matrix;
}

element_t *** sampleRandomOrtonormalBases(lostw_general_params *params,int N) {
  int i, j, k;
  element_t t;

  // create canonical base
  element_t ** canonical_base = malloc(sizeof(element_t *) * N);

  for (i = 0; i < N; i++) {
    canonical_base[i] = malloc(sizeof(element_t) * N);

    for (j = 0; j < N; j++) {
      element_init_same_as(canonical_base[i][j], params->g);
      if (i == j)
	element_set(canonical_base[i][j], params->g);
      else
	element_set0(canonical_base[i][j]);
    }
  }

  element_t ** linear_transformation = sample_uniform_transformation(params->pairing, N);

  // generate B
  element_t ** B = malloc(sizeof(element_t *) * N);
  element_init_same_as(t, canonical_base[0][0]);

  for (i = 0; i < N; i++) {
    B[i] = malloc(sizeof(element_t) * N);
    for (j = 0; j < N; j++) {
      element_init_same_as(B[i][j], canonical_base[0][j]);
      element_mul(B[i][j], canonical_base[0][j], linear_transformation[i][0]);
    }
    for (j = 1; j < N; j++) {
      for (k = 0; k < N; k++) {
	element_mul(t, canonical_base[j][k], linear_transformation[i][j]);
	element_add(B[i][j], B[i][j], t);
      }
    }
  }

  // generate B*
  element_t ** Bstar = malloc(sizeof(element_t *) * N);

  linear_transformation = invert(traspose(linear_transformation, N), N);
  
  for (i = 0; i < N; i++) {
    
  }

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
  
  element_t *** dualOrthonormalBases = sampleRandomOrtonormalBases(genparams, N);
  element_t B[1];

  pbc_param_clear(ec_params);

  return params;
}
