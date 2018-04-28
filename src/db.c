#include "db.h"
#include "sqlite3.h"
#include "util.h"

bool
db_open(const char *file, sqlite3 **db)
{
	debug("Opening DB: %s", file);

	int result = sqlite3_open(file, db);
	if (result) {
		error("Failed to open db: %s", sqlite3_errmsg(*db));
	}
	return result == 0;
}

bool
db_execute_stmnt(const char *stmnt,
		 sqlite3 *db,
		 int(*cb)(void*, int, char**, char**),
		 void *cb_arg)
{
	debug("Executing sql: %s", stmnt);

	char *errorMsg = NULL;
	if (sqlite3_exec(db, stmnt, cb, cb_arg, &errorMsg)) {
		error("Faled to execute statement: %s", stmnt);
		error("Sqlite3: %s", errorMsg);
		sqlite3_free(errorMsg);
		return false;
	}
	return true;
}

void db_close(sqlite3 **db)
{
	debug("Closing db: %s", sqlite3_db_filename(*db, NULL));
	sqlite3_close(*db);
	*db = NULL;
}
