#ifndef DATABASE_H
#define DATABASE_H

#include "database/sqlite.h"


void set_db_type(int dbtype);
unsigned int open_database(const char *dblocation);
unsigned int close_database();

#endif
