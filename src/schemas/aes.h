#ifndef _AES_H
#define _AES_H

#include <pbc/pbc.h>

int encrypt_AES(unsigned char *buffer, element_t k, int *val, int size);

#endif
