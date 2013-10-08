#ifndef UTILS_H
#define UTILS_H

#include <pbc/pbc.h>
#include <string.h>

pairing_t * load_pairing(const char *params_path);

element_t * vector_times_matrix(pairing_t * pairing, element_t *v, element_t **m, int n);

element_t ** transpose(element_t **m, int n);

void minorX(element_t * tmp1, element_t a, element_t b, element_t c, element_t d);

#endif
