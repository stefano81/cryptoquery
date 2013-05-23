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
      //      printf("%d %d -> %s\n", i, j, (element_is0(I[i][j]) ? "true" : "false"));
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
  element_t **cbase, psi, t;

  setup_t out = malloc(sizeof(*out));
  msk_t private = out->private = malloc(sizeof(*private));
  mpk_t public = out->public = malloc(sizeof(*public));

  cbase = create_identity_matrix(pairing, 3);

  element_init_Zr(psi, *pairing);
  element_random(psi);

  for (int i = 0; i < 3; ++i)
    element_mul_zn(cbase[i][i], cbase[i][i], psi); // now \psi-identity -> canonical base


  element_init_G1(out->g, *pairing);
  element_random(out->g);

  private->l = public->l = l;
  private->C = malloc(sizeof(element_t **) * (l + 1));
  public->B = malloc(sizeof(element_t **) * (l + 1));
  

  element_init_GT(t, *pairing);
  element_init_GT(public->gT, *pairing);

  pairing_apply(t, out->g, out->g, *pairing);
  element_pow_zn(public->gT, t, psi);

  element_clear(t);

  element_init_G1(t, *pairing);

  for (int k = 0; k <= l; ++k) {
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
  }

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
  ciphertext_t ct = malloc(sizeof(*ct));
  element_t one, xt, v[3],
    z,
    wt;

  element_init_GT(*m, *pairing);
  element_random(*m);

  ct->l = public->l;

  element_init_Zr(xt, *pairing);

  element_init_G1(one, *pairing);
  element_set1(one);

  ct->ci = malloc(sizeof(element_t) * (ct->l + 1));

  element_init_Zr(z, *pairing);
  element_init_Zr(wt, *pairing);
  element_random(z);

  element_init_GT(ct->c, *pairing);
  element_pow_zn(ct->c, public->gT, z);
  element_mul(ct->c, ct->c, *m);


  element_init_G1(v[0], *pairing);
  element_random(wt); // w_0
  element_mul_zn(v[0], one, wt);

  element_init_G1(v[1], *pairing);
  element_mul_zn(v[1], one, z);

  element_init_G1(v[2], *pairing);
  element_set0(v[2]);

  ct->ci[0] = vector_times_matrix(v, public->B[0], 3);

  element_mul_zn(v[2], one, wt);

  for (int i = 1; i <= ct->l; ++i) {
    element_random(wt); // wt <- F_q
    element_mul_zn(v[0], one, wt);
    element_set_si(xt, x[i-1]);
    element_mul(wt, wt, xt);
    element_mul_zn(v[1], one, wt);

    ct->ci[i]  = vector_times_matrix(v, public->B[i], 3);
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

  element_init_G1(one, *pairing);
  element_init_G1(v[0], *pairing);
  element_init_G1(v[1], *pairing);
  element_init_G1(v[2], *pairing);
  element_init_Zr(dt, *pairing);
  element_init_Zr(yt, *pairing);
  element_init_Zr(s0, *pairing);
  element_init_Zr(st, *pairing);

  element_set1(one);
  element_set0(s0);

  unsigned long mask = 0x01;
  for (int i = 1; i <= private->l; ++i, mask <<= 1) {
    if (-1 == y[i - 1]) {
      k->k[i] = NULL;
      continue;
    }

    k->S |= mask;

    element_random(dt);
    element_random(st);
    element_add(s0, s0, st); // sum st

    element_set_si(yt, y[i - 1]);

    element_mul(yt, dt, yt);
    element_mul_zn(v[0], one, yt);

    element_neg(dt, dt);
    element_mul_zn(v[1], one, dt);

    element_mul_zn(v[2], one, st);

    k->k[i] = vector_times_matrix(v, private->C[i], 3);
  }

  element_neg(v[0], s0); // s0 = - \sum s_t
  element_set1(v[1]);
  element_random(v[2]); // eta

  k->k[0] = vector_times_matrix(v, private->C[0], 3);

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

element_t * decrypt(pairing_t* pairing, ciphertext_t ct, dkey_t key) {
  element_t * m = malloc(sizeof(element_t));
  element_t t1, t2;

  element_init_GT(*m, *pairing);
  element_init_GT(t1, *pairing);
  element_init_GT(t2, *pairing);

  element_prod_pairing(t2, key->k[0], ct->ci[0], 3);

  unsigned long mask = 0x01;
  for (int i = 1; i <= ct->l; ++i, mask <<= 1) {
    if (key->S & mask) {
      // i \in S
      element_prod_pairing(t1, key->k[i], ct->ci[i], 3);
      element_mul(t2, t2, t1);
    }
  }

  element_div(*m, ct->c, t2);

  element_clear(t1);
  element_clear(t2);

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
