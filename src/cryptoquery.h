#ifndef CRYPTOQUERY_H
#define CRYPTOQUERY_H

#include <stdio.h>
#include <stdlib.h>

#include "database.h"
#include "encryption.h"

// schemas
// - symmetric
#define NONE 0x00
#define AES  0x01

// - asymmetric
#define LOSTW 0x00
#define HVE   0x01
#define BPT   0x02

typedef struct {
  int db_type;
  char * db_path;
  int senc;
  int aenc;
  char **encparams;
} configuration;

#endif
