#ifndef DATABASE_H
#define DATABASE_H

#include "common.h"
#include "database/sqlite.h"


void set_db_type(int dbtype);
int open_database(const char *dblocation);

#endif
