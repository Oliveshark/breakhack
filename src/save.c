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
#include <stdbool.h>
#include "save.h"
#include "sqlite3.h"
#include "db.h"
#include "defines.h"
#include "util.h"

static sqlite3 *db = NULL;
static bool loaded = false;
static Save save;

static
DbQuery MIGRATE_COMMAND = {
	"CREATE TABLE IF NOT EXISTS saves("
		"major_version INTEGER, "
		"minor_version INTEGER, "
		"patch_version INTEGER, "
		"save BLOB)",
	NULL, NULL
};

static
DbQuery CLEAR_SAVE = { "DELETE FROM saves", NULL, NULL };

static void
create_table(void)
{
	db_execute(db, &MIGRATE_COMMAND);
}

void
save_load(void)
{
	debug("Loading save");
	const char *query =
		"SELECT save FROM saves "
		"WHERE major_version = ?"
		"AND minor_version = ?"
		"AND patch_version = ?"
		"LIMIT 1";

	sqlite3_stmt *stmt = db_prepare(db, query);
	sqlite3_bind_int(stmt, 1, MAJOR_VERSION);
	sqlite3_bind_int(stmt, 2, MINOR_VERSION);
	sqlite3_bind_int(stmt, 3, PATCH_VERSION);
	if (SQLITE_ROW == sqlite3_step(stmt)) {
		int size = sqlite3_column_bytes(stmt, 0);
		debug("Reading save bytes: %d", size);
		memcpy(&save, sqlite3_column_blob(stmt, 0), size);
		loaded = true;
	} else {
		loaded = false;
	}
	sqlite3_finalize(stmt);
}

void
save_init(void)
{
	if (!db_open(DB_FILE, &db)) {
		db_close(&db);
		fatal("Exiting");
	}
	create_table();
	save_load();
}

const Save *
save_get(void)
{
	return &save;
}

bool
save_exists(void)
{
	return loaded;
}

void
save_save(unsigned int seed,
	  unsigned int map_level,
	  bool quickGame,
	  bool arcadeGame,
	  Player *player)
{
	debug("Saving game, Seed: %d, Map level: %d", seed, map_level);
	save_clear();

	save.seed = seed;
	save.map_level = map_level;
	save.quickGame = quickGame;
	save.arcadeGame = arcadeGame;
	save.player_stats = player->stats;
	save.player_daggers = player->daggers;
	save.player_gold = player->gold;
	save.player_xp = player->xp;
	save.player_potion_sips = player->potion_sips;
	save.player_player_stats = player->stat_data;
	save.player_state = player->stateData;
	save.player_class = player->class;
	save.player_equipment = player->equipment;

	const char *query =
		"INSERT INTO saves"
		"(major_version, minor_version, patch_version, save) "
		"VALUES(?, ?, ?, ?)";

	sqlite3_stmt *stmt = db_prepare(db, query);
	sqlite3_bind_int(stmt, 1, MAJOR_VERSION);
	sqlite3_bind_int(stmt, 2, MINOR_VERSION);
	sqlite3_bind_int(stmt, 3, PATCH_VERSION);
	sqlite3_bind_blob(stmt, 4, &save, sizeof(Save), SQLITE_STATIC);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

void
save_clear(void)
{
	db_execute(db, &CLEAR_SAVE);
}

void
save_close(void)
{
	db_close(&db);
}
