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
  int i, j;

  for (i = y; i < size_y; i++) {
    for (j = x; j < size_x; j++) {
      element_set(d[i - y][j - x], s[i][j]);
    }
  }
}

element_t ** inv(element_t ** m, int n) {
  int i, j, k;
  int n2 = 2 * n;
  element_t a, b, t;

  element_init_same_as(a, m[0][0]);
  element_init_same_as(b, m[0][0]);
  element_init_same_as(t, m[0][0]);

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
      exit(1);
    } else {
      // normalize the vector
      for (j = 0; j < n2; j++) {
	element_div(m[i][j], m[i][j], a);
      }

      // subtract the vector from all other vectors to zero the
      // relevant matrix elements in the current column
      for (k = 0; k < n; k++) {
	if (0 != (k - i)) {
	  element_set(b, m[k][j]);
	  for (j = 0; j < n2; j++) {
	    element_mul(t, b, m[i][j]);
	    element_sub(m[k][j], m[k][j], t);
	  }
	}
      }
    }
  }

  element_clear(a);
  element_clear(b);
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
	element_add(B[i][k], B[i][k], t);
      }
    }
  }

  // generate B*
  element_t ** Bstar = malloc(sizeof(element_t *) * N);

  linear_transformation = invert(traspose(linear_transformation, N), N);
  
  for (i = 0; i < N; i++) {
    Bstar[i] = malloc(sizeof(element_t) * N);
    for (j = 0; j < N; j++) {
      element_init_same_as(Bstar[i][j], canonical_base[0][0]);
      element_mul(Bstar[i][j], canonical_base[0][j], linear_transformation[0][i]);
    }
    for (j = 0; j < N; j++) {
      for (k = 0; k < N; k++) {
	element_mul(t, canonical_base[j][k], linear_transformation[i][j]);
	element_add(Bstar[i][k], B[i][k], t);
      }
    }
  }

  element_t *** bases = malloc(sizeof(element_t **) * 2);

  bases[0] = B;
  bases[1] = Bstar;

  element_clear(t);

  return bases;
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
  element_t ** B = public->B =  malloc(sizeof(element_t *) * (n + 2));
  element_t ** BStar = secret->BStar = malloc(sizeof(element_t *) * (n + 2));

  int i, j;

  for (i = 0; i < n; i++) {
    B[i] = malloc(sizeof(element_t) * n);
    BStar[i] = malloc(sizeof(element_t) * n);
    for (j = 0; j < n; j++) {
      element_init_same_as(B[i][j], dualOrthonormalBases[0][0][0]);
      element_set(B[i][j], dualOrthonormalBases[0][i][j]);

      element_init_same_as(BStar[i][j], dualOrthonormalBases[0][0][0]);
      element_set(BStar[i][j], dualOrthonormalBases[1][i][j]);
    }
  }

  B[n] = malloc(sizeof(element_t) * n);
  B[n + 1] = malloc(sizeof(element_t) * n);
  BStar[n] = malloc(sizeof(element_t) * n);
  BStar[n + 1] = malloc(sizeof(element_t) * n);

  for (i = 0; i < n; i++) {
    element_init_same_as(B[n][i], dualOrthonormalBases[0][0][0]);
    element_set(B[n][i], dualOrthonormalBases[0][N - 3][i]);
    element_init_same_as(B[n + 1][i], dualOrthonormalBases[0][0][0]);
    element_set(B[n + 1][i], dualOrthonormalBases[0][N - 1][i]);

    element_init_same_as(BStar[n][i], dualOrthonormalBases[0][0][0]);
    element_set(BStar[n][i], dualOrthonormalBases[1][N - 3][i]);
    element_init_same_as(BStar[n + 1][i], dualOrthonormalBases[0][0][0]);
    element_set(BStar[n + 1][i], dualOrthonormalBases[1][N - 2][i]);
  }

  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      element_clear(dualOrthonormalBases[0][i][j]);
      element_clear(dualOrthonormalBases[1][i][j]);
    }
    free(dualOrthonormalBases[0][i]);
    free(dualOrthonormalBases[1][i]);
  }
  free(dualOrthonormalBases[0]);
  free(dualOrthonormalBases[1]);

  free(dualOrthonormalBases);
  pbc_param_clear(ec_params);

  return params;
}

lostw_key * lostw_keygen(lostw_general_params * params, lostw_msk * secret, element_t *v) {
  lostw_key * key = malloc(sizeof(lostw_key));
  element_t sigma, eta, * t, * kstar, ts;
  int i, j, n = params->n;

  element_init_Zr(sigma, params->pairing);
  element_random(sigma);
  element_init_Zr(eta, params->pairing);
  t = malloc(sizeof(element_t) * n);
  kstar = key->kstar = malloc(sizeof(element_t) * n);

  for (i = 0; i < n; i++) {
    element_init_Zr(t[i], params->pairing);
    element_init_Zr(kstar[i], params->pairing);
    element_set0(kstar[i]);
  }

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      element_mul(t[j], v[i], secret->BStar[i][j]);
      element_mul(t[j], sigma, t[j]);
      element_add(kstar[j], kstar[j], t[j]);
    }
  }
  element_init_same_as(ts, t[0]);
  for (i = 0; i < n; i++) {
    element_add(kstar[i], kstar[i], secret->BStar[n][i]);
    element_mul(ts, eta, secret->BStar[n + 1][i]);
    element_add(kstar[i], kstar[i], ts);
  }

  for (i = 0; i < n; i++) {
    element_clear(t[i]);
  }
  element_clear(ts);

  free(t);
  
  return key;
}
