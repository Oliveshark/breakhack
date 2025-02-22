/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2025  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
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

sqlite3_stmt *
db_prepare(sqlite3 *db, const char *query)
{
	const char *pzTest;
	sqlite3_stmt *stmt;
	int result;

	result = sqlite3_prepare_v2(db,
				    query,
				    (int) strlen(query),
				    &stmt,
				    &pzTest);
	if (result != SQLITE_OK)
		fatal("Failed to prepare statement: %s", query);

	return stmt;
}

void
db_execute(sqlite3 *db, DbQuery *query)
{
	if (!db_execute_stmnt(query->stmt, db, query->cb, query->cb_arg)) {
		db_close(&db);
		fatal("Exiting");
	}
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
		error("Failed to execute statement: %s", stmnt);
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
