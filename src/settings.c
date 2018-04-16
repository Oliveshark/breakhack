/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
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

#include <stdlib.h>
#include <string.h>
#include "settings.h"
#include "sqlite3.h"
#include "util.h"
#include "defines.h"
#include "db.h"

#define SETTINGS_DB_FILE ".data.db"

static sqlite3 *db = NULL;
static Settings settings;

static const char *KEY_MUSIC_ENABLED = "music_enabled";
static const char *KEY_SOUND_ENABLED = "sound_enabled";

static void
set_default_settings(void)
{
	settings.music_enabled = true;
	settings.sound_enabled = true;
}

static void
execute_statement(const char *stmt,
	int(*cb)(void*, int, char**, char**),
	void *cb_arg)
{
	if (!db_execute_stmnt(stmt, db, cb, cb_arg)) {
		db_close(&db);
		fatal("Exiting");
	}
}

static void
create_tables(void)
{
	execute_statement("CREATE TABLE IF NOT EXISTS settings_int(key TEXT PRIMARY KEY, value INTEGER)",
		NULL,
		NULL);
}

static int
load_settings_cb(void *unused, int count, char **values, char **colNames)
{
	UNUSED(unused);

	int i = 0;
	while (i < count) {
		if (strcmp("key", colNames[i]) != 0)
			continue;

		debug("Loading setting: %s = %s", values[i], values[i + 1]);
		if (!strcmp(KEY_SOUND_ENABLED, values[i])) {
			settings.sound_enabled = (bool)atoi(values[i + 1]);
			i += 2;
		}
		else if (!strcmp(KEY_MUSIC_ENABLED, values[i])) {
			settings.music_enabled = (bool)atoi(values[i + 1]);
			i += 2;
		}
	}
	return 0;
}

static void
load_settings(void)
{
	execute_statement("SELECT * FROM settings_int", load_settings_cb, NULL);
}

void
settings_init(void)
{
	if (!db_open(SETTINGS_DB_FILE, &db)) {
		db_close(&db);
		fatal("Exiting");
	}
	set_default_settings();
	create_tables();
	load_settings();
}

static void
save_setting_int(const char *key, int value)
{
	// TODO(Linus): Move this into db.c, probably using varargs

	const char *stmtStr = "INSERT OR REPLACE INTO settings_int(key, value) values (?, ?)";
	const char *pzTest;
	sqlite3_stmt *stmt;
	int result;

	result = sqlite3_prepare_v2(db, stmtStr, (int) strlen(stmtStr), &stmt, &pzTest);

	debug("Saving setting: %s = %d", key, value);
	if (result == SQLITE_OK) {
		sqlite3_bind_text(stmt,
				  1,
				  key,
				  (int) strlen(key),
				  NULL);

		sqlite3_bind_int(stmt, 2, value);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	} else {
		error("Failed to prepare storage statement for: %s", key);
	}
}

static void
settings_save(void)
{
	save_setting_int(KEY_SOUND_ENABLED, settings.sound_enabled);
	save_setting_int(KEY_MUSIC_ENABLED, settings.music_enabled);
}

Settings *
settings_get(void)
{
	return &settings;
}

void
settings_close(void)
{
	settings_save();
	db_close(&db);
}
