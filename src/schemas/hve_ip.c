#include "hve_ip.h"
#include <stdio.h>
#include <stdlib.h>

pairing_t * load_pairing(char *params_path) {
  char param[1024];
  pairing_t *pairing = malloc(sizeof(pairing_t));

  FILE * file = fopen(params_path, "r");
  size_t read = fread(param, 1, 1024, file);
  fclose(file);

  if (!read) pbc_die("Error reading the parameter file");

  pairing_init_set_buf(*pairing, param, read);

  return pairing;
}

static element_t ** create_identity_matrix(pairing_t *pairing, int n) {
  element_t **I = malloc(sizeof(element_t *) * n);

  for (int i = 0; i < n; ++i) {
    I[i] = malloc(sizeof(element_t) * n);
    for (int j = 0; j < n; ++j) {
      element_init_G1(I[i][j], *pairing);
      if (i == j)
	element_set1(I[i][j]);
      else
	element_set0(I[i][j]);
    }
  }

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

static element_t ** transpose(element_t **m, int n) {
  element_t t;
  element_init_same_as(t, m[0][0]);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      element_set(t, m[i][j]);
      element_set(m[i][j], m[j][i]);
      element_set(m[j][i], t);
    }
  }

  element_clear(t);

  return m;
}

static copy(element_t **dst, element_t **src, int sizeY, int sizeX, int x, int y) {
  for (int i = y; i < sizeY; ++i) {
    for (int j = x; j < sizeX; ++j) {
      element_set(dst[i - y][j -x], src[i][j]);
    }
  }
}

static element_t ** invert(element_t **m, int n) {
  element_t **tm = malloc(sizeof(element_t*) * n);

  for (int i = 0; i < n; ++i) {
    tm[i] = malloc(sizeof(element_t)*  2 * n);
    for (int j = 0; j < n; ++j) {
      element_init(tm[i][j], m[0][0]);
    }
  }

  copy(tm, m, n, n, 0, 0);
  //invert2(tm, n);
  copy(m, tm, n, 2*n, 0, n);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      element_clear(tm[i][j]);
    }
    free(tm[i]);
  }

  free(tm);

  return m;
}

setup_t setup(pairing_t* pairing, int l) {
  element_t ** cbase = create_identity_matrix(pairing, 3);
  element_t psi;
  ++l;

  element_init_G1(psi, *pairing);
  element_random(psi);

  for (int i = 0; i < 3; ++i)
    element_mul(cbase[i][i], cbase[i][i], psi); // now \psi-identity -> canonical base

  setup_t out = malloc(sizeof(*out));
  msk_t private = out->private = malloc(sizeof(*private));
  mpk_t public = out->public = malloc(sizeof(*public));

  private->l = public->l = l;
  private->C = malloc(sizeof(element_t **) * l);
  public->B = malloc(sizeof(element_t **) * l);
  
  element_t t;
  element_init_G1(t, *pairing);

  for (int k = 0; k < l; ++k) {
      element_t ** lt = sample_linear_transformation_Zr(pairing, 3);

      element_t ** B = public->B[k] = malloc(sizeof(element_t *) * 3);

      for (int i = 0; i < 3; ++i) {
	B[i] = malloc(sizeof(element_t) * 3);

	for (int j = 0; j < 3; ++j) {
	  element_init_G1(B[i][j], *pairing);
	  element_mul_zn(B[i][j], cbase[0][j], lt[i][0]);
	}
	for (int r = 1; r < 3; ++r) {
	  for (int j = 0; j < 3; ++j) {
	    element_mul_zn(t, cbase[r][j], lt[i][r]);
	    element_add(B[i][j], B[i][j], t);
	  }
	}
      }

      lt = invert(transpose(lt, 3), 3);

      element_t ** C = private->C[k] = malloc(sizeof(element_t*) * 3);

      for (int i = 0; i < 3; ++i) {
	C[i] = malloc(sizeof(element_t) * 3);

	for (int j = 0; j < 3; ++j) {
	  element_init_G1(C[i][j], *pairing);
	  element_mul_zn(C[i][j], cbase[0][j], lt[i][0]);
	}
	for (int r = 1; r < 3; ++r) {
	  for (int j = 0; j < 3; ++j) {
	    element_mul_zn(t, cbase[r][j], lt[i][r]);
	    element_add(C[i][j], C[i][j], t);
	  }
	}
      }

      for (int i = 0; i < 3; ++i) {
	for (int j = 0; j < 3; ++j)
	  element_clear(lt[i][j]);
	free(lt[i]);
      }
      free(lt);

      // check if \psi-orthonormal
      
  }

  // clean up
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j)
      element_clear(cbase[i][j]);

    free(cbase[i]);
  }

  free(cbase);
  element_clear(psi);
  element_clear(t);

  return out;
}

ciphertext_t encrypt(pairing_t* pairing, mpk_t public, int x[], element_t *m) {

}

key_t keygen(pairing_t* pairing, msk_t private, int y[]) {

}

element_t * decript(pairing_t* pairing, ciphertext_t ct, key_t key) {

}


