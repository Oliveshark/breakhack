#ifndef DB_H_
#define DB_H_

#include <stdbool.h>
#include "sqlite3.h"

bool
db_open(const char *file, sqlite3 **db);

bool
db_execute_stmnt(const char *stmnt,
		 sqlite3 *db,
		 int(*cb)(void*, int, char**, char**),
		 void *cb_arg);

void
db_close(sqlite3 **db);

#endif // DB_H_