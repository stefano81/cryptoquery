#ifndef CQ_COMMON_H
#define CQ_COMMON_H

#include <stdio.h>
#include <stdlib.h>

#define CQ_OK 0x00
#define CQ_ERROR 0xFF

// commands
#define CQ_QUIT 0x00
#define CQ_ADD 0x01
#define CQ_SELECT 0x02

#define CQ_NO_COMMAND 0xFF

extern unsigned int verbose;

#endif
