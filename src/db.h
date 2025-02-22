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

#ifndef DB_H_
#define DB_H_

#include <stdbool.h>
#include "sqlite3.h"

#define DB_FILE ".data.db"

typedef struct DbQuery {
	char *stmt;
	int (*cb)(void*, int, char**, char**);
	void *cb_arg;
} DbQuery;


bool
db_open(const char *file, sqlite3 **db);

sqlite3_stmt*
db_prepare(sqlite3*, const char *query);

void
db_execute(sqlite3*, DbQuery*);

bool
db_execute_stmnt(const char *stmnt,
		 sqlite3 *db,
		 int(*cb)(void*, int, char**, char**),
		 void *cb_arg);

void
db_close(sqlite3 **db);

#endif // DB_H_

