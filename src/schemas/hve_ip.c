#include "hve_ip.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

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
      printf("%d %d -> %s\n", i, j, (element_is0(I[i][j]) ? "true" : "false"));
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
  
  for (int i = 0; i < n; ++i) {
    tm[i] = malloc(sizeof(element_t)*  2 * n);
    for (int j = 0; j < n * 2; ++j) {
      element_init_same_as(tm[i][j], m[0][0]);
    }
  }

  copy(tm, m, n, n, 0, 0);
  invert2(tm, n);
  copy(m, tm, n, 2 * n, 0, n);
  
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n * 2; ++j) {
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

  printf("Identity:\n");
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j)
      printf(" %c", element_is0(cbase[i][j]) ? '0': element_is1(cbase[i][j]) ? '1' : 'x');
    printf("\n");
  }
  printf(" -----\n");


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

      // check if \psi-orthonormal using lt as a support
      for (int i = 0; i < 3; ++i) {
	for (int j = 0; j < 3; ++j) {
	  element_set0(lt[i][j]);
	  for (int r = 0; r < 3; ++r) {
	    element_mul(t, B[i][r], C[r][j]);
	    element_add(lt[i][j], lt[i][j], t);
	  }
	}
      }

      for (int i = 0; i < 3; ++i) {
	for (int j = 0; j < 3; ++j)
	  printf(" %c", element_is0(lt[i][j]) ? '0': element_is1(lt[i][j]) ? '1' : 'x');
	for (int j = 0; j < 3; ++j)
	  printf(" %c", element_is0(B[i][j]) ? '0': element_is1(C[i][j]) ? '1' : 'x');
	for (int j = 0; j < 3; ++j)
	  printf(" %c", element_is0(C[i][j]) ? '0': element_is1(C[i][j]) ? '1' : 'x');
	printf("\n");
      }
      printf(" ----- ----- -----\n");

      for (int i = 0; i < 3; ++i) {
	for (int j = 0; j < 3; ++j)
	  element_clear(lt[i][j]);
	free(lt[i]);
      }
      free(lt);
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

ciphertext_t encrypt(pairing_t* pairing, mpk_t public, unsigned int x[], element_t *m) {
  element_random(*m);

  ciphertext_t ct = malloc(sizeof(*ct));
  ct->l = public->l;

  element_t * X = malloc(sizeof(element_t) * ct->l);

  for (int i = 0; i < ct->l; ++i) {
    element_init_Zr(X[i], *pairing);
    element_set_si(X[i], x[i]);
  }

  ct->ci = malloc(sizeof(element_t) * (ct->l + 1));
  
  element_t z, *w = malloc(sizeof(element_t) * (ct->l + 1));

  element_init_Zr(z, *pairing);
  element_random(z);

  for (int i = 0; i <= ct->l; ++i) {
    element_init_G1(w[i], *pairing);
    element_random(w[i]);
  }

  element_pow_zn(ct->c, public->g_T, z);
  element_mul(ct->c, ct->c, *m);

  element_t t;

  element_init_G1(t, *pairing);

  element_t v[3];
  element_init_G1(v[0], *pairing);
  element_set(v[0], w[0]);

  element_init_G1(v[1], *pairing);
  element_set1(t);
  element_mul_zn(v[1], t, z);

  element_init_G1(v[2], *pairing);
  element_set0(v[2]);

  ct->ci[0] = vector_times_matrix(v, public->B[0], 3);

  element_set(v[2], w[0]);
  for (int i = 1; i <= ct->l; ++i) {
    element_set(v[0], w[i]);
    element_mul_zn(v[1], w[i], X[i]);

    ct->ci[i]  = vector_times_matrix(v, public->B[i], 3);
  }

  return ct;
}

dkey_t keygen(pairing_t* pairing, msk_t private, int y[]) {
  dkey_t k = malloc(sizeof(*k));

  element_t *Y = malloc(sizeof(element_t) * private->l),
    *s = malloc(sizeof(element_t) * (private->l)),
    *d = malloc(sizeof(element_t) * (private->l)),
    s0;

  for (int i = 0; i < private->l; ++i) {
    if (-1 == y[i]) {
      Y[i] = NULL;
      continue;
    }

    element_init_G1(s[i], *pairing);
    element_random(s[i]);
    element_init_G1(d[i], *pairing);
    element_init_G1(Y[i], *pairing);
  }

  return k;
}

element_t * decript(pairing_t* pairing, ciphertext_t ct, dkey_t key) {

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
