#include "lostw.h"
/*
void init_pairing_params(pairing_t * pairing) {
  pbc_param_t ec_params;
  unsigned int rbits = 180, qbits = 603;

  pbc_param_init_a_gen(ec_params, rbits, qbits);
  pairing_init_pbc_param(*pairing, ec_params);
  pbc_param_clear(ec_params);
}
 */

void init_pairing_params(pairing_t * pairing, const char *path) {
  char param[1024];
  FILE * file = fopen(path, "r");
  size_t count = fread(param, 1, 1024, file);
  fclose(file);

  if (!count) pbc_die("Error reading from file");
  
  pairing_init_set_buf(*pairing, param, count);
}

element_t ** sample_uniform_linear_transformation_Zr(pairing_t pairing, unsigned int n) {
  element_t ** m = malloc(sizeof(element_t *) * n);

  for (int i = 0; i < n; i++) {
    m[i] = malloc(sizeof(element_t) * n);
    for (int j = 0; j < n; j++) {
      element_init_Zr(m[i][j], pairing);
      element_random(m[i][j]);
    }
  }

  return m;
}

void array_copy(element_t ** d, element_t ** s, int sizey, int sizex, int y, int x) {
  for (int i = y; i < sizey; i++) {
    for (int j = x; j < sizex; j++) {
      element_set(d[i - y][j - x], s[i][j]);
    }
  }
}

element_t ** invert_array(element_t ** m, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      element_set0(m[i][j + n]);
    }
    element_set1(m[i][i + n]);
  }

  element_t a, b;
  element_init_same_as(a, m[0][0]);

  int n2 = n * 2;

  for (int i = 0; i < n; i++) {
    element_set(a, m[i][i]);
    if (element_is0(a)) {
      fprintf(stderr, "ERROR: alpha is zero!\n");
      exit(1);
    }

    // normalize
    for (int j = 0; j < n2; j++) {
      element_div(m[i][j], m[i][j], a);
    }

    // subtract the vector from all other vectors to zero the
    // relevant matrix elements in the current column
    for (int k = 0; k < n; k++) {
      if (0 != (k - i)) {
	element_set(b, m[k][i]);

	for (int j = 0; j < n2; j++) {
	  element_mul(a, b, m[i][j]);
	  element_sub(m[k][j], m[k][j], a);
	}
      }
    }
  }

  element_clear(a);
  element_clear(b);
}

element_t ** invert(element_t ** m, unsigned int n) {
  element_t ** tm = malloc(sizeof(element_t *) * n);

  for (int i = 0; i < n; i++) {
    tm[i] = malloc(sizeof(element_t) * (2 * n));
    for (int j = 0; j < (2 * n); j++)
      element_init_same_as(tm[i][j], m[0][0]);
  }

  array_copy(tm, m, n, n, 0, 0);
  array_copy(m, invert_array(tm, n), n, 2 * n, 0, n);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < (2 * n); j++) {
      element_clear(tm[i][j]);
    }
    free(tm[i]);
  }

  free(tm);

  return m;
}

element_t ** traspose(element_t ** m, unsigned int n) {
  element_t t;
  element_init_same_as(t, m[0][0]);

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      element_set(t, m[i][j]);
      element_set(m[i][j], m[j][i]);
      element_set(m[j][i], t);
    }

  element_clear(t);

  return m;
}

element_t *** sample_dual_orthonormal_bases(lostw_general_params * params, unsigned int n) {
  element_t *** bases = malloc(sizeof(element_t **) * 2);
  element_t ** B = bases[0] = malloc(sizeof(element_t *) * n), ** BStar = bases[1] = malloc(sizeof(element_t *) * n);
  element_t ** cb = malloc(sizeof(element_t *) * n);
  
  for (int i = 0; i < n; i++) {
    cb[i] = malloc(sizeof(element_t) * n);
    B[i] = malloc(sizeof(element_t) * n);
    BStar[i] = malloc(sizeof(element_t) * n);
    for (int j = 0; j < n; j++) {
      element_init_G1(cb[i][j], params->pairing);

      if (i == j)
	element_set(cb[i][j], params->g);
      else
	element_set0(cb[i][j]);

      element_init_G1(B[i][j], params->pairing);
      element_init_G1(BStar[i][j], params->pairing);
    }
  }

  element_t ** lt = sample_uniform_linear_transformation_Zr(params->pairing, n);
  element_t t;
  element_init_G1(t, params->pairing);

  for (int i = 0; i < n; i++) {
    for (int k = 0; k < n; k++) {
      element_mul_zn(B[i][k], cb[0][k], lt[i][0]);
    }
    for (int j = 1; j < n; j++) {
      for (int k = 0; k < n; k++) {
	element_mul_zn(t, cb[j][k], lt[i][j]);
	element_add(B[i][k], B[i][k], t);
      }
    }
  }

  lt = invert(traspose(lt, n), n);

  for (int i = 0; i < n; i++) {
    for (int k = 0; k < n; k++) {
      element_mul_zn(BStar[i][k], cb[0][k], lt[i][0]);
    }
    for (int j = 1; j < n; j++) {
      for (int k = 0; k < n; k++) {
	element_mul_zn(t, cb[j][k], lt[i][j]);
	element_add(BStar[i][k], BStar[i][k], t);
      }
    }
  }

  element_clear(t);
  

  return bases;
}

lostw_setup_param * lostw_setup(unsigned int n, const char * path) {
  lostw_setup_param * setup_params = malloc(sizeof(lostw_setup_param));
  lostw_general_params * params = setup_params-> params = malloc(sizeof(lostw_general_params));
  lostw_mpk * public = setup_params->public = malloc(sizeof(lostw_mpk));
  lostw_msk * secret = setup_params->secret = malloc(sizeof(lostw_msk));

  //init_pairing_params(&(params->pairing));
  init_pairing_params(&params->pairing, path);
  params->n = n;
  element_init_G1(params->g, params->pairing);
  element_random(params->g);

  unsigned int N = 2 * n + 3;
  
  element_t *** b_bstar = sample_dual_orthonormal_bases(params, N);

  public->B = malloc(sizeof(element_t *) * (n + 2));
  secret->BStar = malloc(sizeof(element_t *) * (n + 2));

  for (int i = 0; i < n; i++) {
    public->B[i] = malloc(sizeof(element_t) * n);
    secret->BStar[i] = malloc(sizeof(element_t) * n);
    for (int j = 0; j < n; j++) {
      element_init_same_as(public->B[i][j], b_bstar[0][0][0]);
      element_init_same_as(secret->BStar[i][j], b_bstar[1][0][0]);
      element_set(public->B[i][j], b_bstar[0][i][j]);
      element_set(secret->BStar[i][j], b_bstar[1][i][j]);
    }
  }
  public->B[n] = malloc(sizeof(element_t) * n);
  public->B[n + 1] = malloc(sizeof(element_t) * n);
  secret->BStar[n] = malloc(sizeof(element_t) * n);
  secret->BStar[n + 1] = malloc(sizeof(element_t) * n);
  for (int i = 0; i < n; i++) {
    element_init_same_as(public->B[n][i], b_bstar[0][0][0]);
    element_init_same_as(public->B[n + 1][i], b_bstar[0][0][0]);
    element_set(public->B[n][i], b_bstar[0][N - 3][i]);
    element_set(public->B[n + 1][i], b_bstar[0][N - 1][i]);

    element_init_same_as(secret->BStar[n][i], b_bstar[1][0][0]);
    element_init_same_as(secret->BStar[n + 1][i], b_bstar[1][0][0]);
    element_set(secret->BStar[n][i], b_bstar[1][N - 3][i]);
    element_set(secret->BStar[n + 1][i], b_bstar[1][N - 2][i]);
  }

  return setup_params;
}

lostw_key * lostw_keygen(lostw_general_params * params, lostw_msk * private, element_t * y) {
  return NULL;
}

lostw_ct * lostw_enc(lostw_general_params * params, lostw_mpk * private, element_t * x, element_t * m) {
  return NULL;
}
