#include "utils.h"

pairing_t * load_pairing(const char *params_path) {
  char param[1024 * 8];
  pairing_t *pairing = malloc(sizeof(pairing_t));

  FILE * file = fopen(params_path, "r");
  size_t read = fread(param, 1, 1024, file);
  fclose(file);

  if (!read) pbc_die("Error reading the parameter file");

  pairing_init_set_buf(*pairing, param, read);

  return pairing;
}

element_t * vector_times_matrix(pairing_t *pairing, element_t *v, element_t **m, int n) {
  element_t *result = malloc(sizeof(element_t) * 3);
  element_t t;

  element_init_same_as(t, m[0][0]);
  element_init_same_as(result[0], m[0][0]);
  element_init_same_as(result[1], m[0][0]);
  element_init_same_as(result[2], m[0][0]);
  /*  element_set0(result[0]);
  element_set0(result[1]);
  element_set0(result[2]);*/

  element_mul_zn(result[0], m[0][0], v[0]);
  element_mul_zn(result[1], m[0][1], v[0]);
  element_mul_zn(result[2], m[0][2], v[0]);

  element_mul_zn(t, m[1][0], v[1]);
  element_add(result[0], result[0], t);
  element_mul_zn(t, m[1][1], v[1]);
  element_add(result[1], result[1], t);
  element_mul_zn(t, m[1][2], v[1]);
  element_add(result[2], result[2], t);

  element_mul_zn(t, m[2][0], v[2]);
  element_add(result[0], result[0], t);
  element_mul_zn(t, m[2][1], v[2]);
  element_add(result[1], result[1], t);
  element_mul_zn(t, m[2][2], v[2]);
  element_add(result[2], result[2], t);

  return result;
}

/*element_t * vector_times_matrix(pairing_t *pairing, element_t *v, element_t **m, int n) {
  element_t * result = malloc(sizeof(element_t) * n);
  element_t t, one;

  element_init_same_as(t, m[0][0]);
  element_init_same_as(t, m[0][0]);//*pairing);
  element_init_same_as(one, m[0][0]);// *pairing);
  element_set1(one);

  for (int i = 0; i < n; ++i) {
    element_init_same_as(result[i], m[0][0]);//v[0]);
    //element_init_G1(result[i], *pairing);

    element_mul_zn(result[i], one, v[0]);
    element_mul_zn(result[i], result[i], m[0][i]);
    //element_mul(result[i], v[0], m[0][i]);
  }

  for (int i = 1; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      element_mul_zn(t, one, v[i]);
      element_mul_zn(t, t, m[i][j]);
      //element_mul(t, v[i], m[i][j]);
      element_add(result[j], result[j], t);
    }
  }
  element_clear(t);
  element_clear(one);

  return result;
  }*/

void minorX(element_t * tmp1, element_t a, element_t b, element_t c, element_t d){
  element_t tmp2;

  element_init_same_as(*tmp1,a);
  element_init_same_as(tmp2,a);
  element_mul(*tmp1,a,b);
  element_mul(tmp2,c,d);
  element_sub(*tmp1,*tmp1,tmp2);

  element_clear(tmp2);
}


element_t ** transpose(element_t **m, int n) {
  element_t t;
  element_init_same_as(t, m[0][0]);

  for (int i = 0; i < (n - 1); ++i) {
    for (int j = i + 1; j < n; ++j) {
      element_set(t, m[i][j]);
      element_set(m[i][j], m[j][i]);
      element_set(m[j][i], t);
    }
  }

  element_clear(t);

  return m;
}

