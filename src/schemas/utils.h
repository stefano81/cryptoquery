#ifndef UTILS_H
#define UTILS_H

#include <pbc/pbc.h>

pairing_t * load_pairing(char *params_path);

element_t * vector_times_matrix(element_t *v, element_t **m, int n);

#endif
