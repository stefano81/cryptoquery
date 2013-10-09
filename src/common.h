#ifndef CQ_COMMON_H
#define CQ_COMMON_H

#include <stdio.h>
#include <stdlib.h>

#define CQ_OK 0x00
#define CQ_ERROR 0xFF

// commands
#define CQ_QUIT   0x00
#define CQ_INSERT 0x01
#define CQ_SELECT 0x02
#define CQ_INIT   0x04
#define CQ_KEYGEN 0x08

extern unsigned int verbose;

#endif
