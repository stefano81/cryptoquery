#include "utils.h"

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

element_t * vector_times_matrix(element_t *v, element_t **m, int n) {
  element_t * result = malloc(sizeof(element_t) * n);
  element_t t;

  element_init_same_as(t, v[0]);

  for (int i = 0; i < n; ++i) {
    element_init_same_as(result[i], v[0]);

    element_mul(result[i], v[0], m[0][i]);
  }

  for (int i = 1; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      element_mul(t, v[i], m[i][j]);
      element_add(result[j], result[j], t);
    }
  }
  element_clear(t);

  return result;
}
