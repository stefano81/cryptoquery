#ifndef _AES_H
#define _AES_H

#include <pbc/pbc.h>

unsigned char * encrypt_AES(element_t k, int *val, int size);
int * decrypt_AES(element_t k, unsigned char *buffer, int size);

#endif
