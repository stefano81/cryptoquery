#include "utils.h"

#define BUF_SIZE (1024 * 8)

pairing_t * load_pairing(const char *params_path) {
  char param[BUF_SIZE];
  pairing_t *pairing = malloc(sizeof(pairing_t));

  FILE * file = fopen(params_path, "r");
  size_t read = fread(param, 1, BUF_SIZE, file);
  fclose(file);

  if (!read) pbc_die("Error reading the parameter file");

  pairing_init_set_buf(*pairing, param, read);

  return pairing;
}

static pbc_param_t * generateA(const char **argv) {
  pbc_param_t * params = malloc(sizeof(pbc_param_t));

  pbc_param_init_a_gen(*params, atoi(argv[0]) // rbits
		       , atoi(argv[1]) // qbits
		       );

  return params;
}


static pbc_param_t * generateA1(const char **argv) {
  pbc_param_t *params = malloc(sizeof(pbc_param_t));
  mpz_t n;

  mpz_init(n);
  mpz_init_set_si(n, atoi(argv[0]));

  pbc_param_init_a1_gen(*params, n);

  return params;
}

pairing_t * create_pairing_type(const char **argv) {
  pairing_t *pairing = malloc(sizeof(pairing_t));
  pbc_param_t *params;

  if (0 == strcmp("A", argv[0])) {
    params = generateA(argv + 1);
  } else if (0 == strcmp("A1", argv[0])) {
    params = generateA1(argv + 1);
  } else if (0 == strcmp("E", argv[0])) {
    
  } else if (0 == strcmp("F", argv[0])) {
  } else if (0 == strcmp("D", argv[0])) {
  }

  pairing = NULL;// something, check the manual

  return pairing;
}

element_t * vector_times_matrix(pairing_t *pairing, element_t *v, element_t **m, int n) {
  element_t *result = malloc(sizeof(element_t) * 3);
  element_t t;

  element_init_same_as(t, m[0][0]);
  element_init_same_as(result[0], m[0][0]);
  element_init_same_as(result[1], m[0][0]);
  element_init_same_as(result[2], m[0][0]);

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

  element_clear(t);

  return result;
}

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

