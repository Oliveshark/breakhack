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

#include <string.h>
#include "hiscore.h"
#include "sqlite3.h"
#include "db.h"
#include "util.h"

static
DbQuery MIGRATE_COMMANDS[] = {
	{
		"CREATE TABLE IF NOT EXISTS hiscore("
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"time DATETIME DEFAULT CURRENT_TIMESTAMP, "
			"gold FLOAT, "
			"playerLevel INTEGER, "
			"dungeonLevel INTEGER)",
		NULL, NULL
	},
	{
		"CREATE TABLE IF NOT EXISTS hiscore_artifacts("
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"hiscoreId INTEGER, "
			"artifactId INTEGER, "
			"FOREIGN KEY(hiscoreId) REFERENCES hiscore(id))",
		NULL, NULL
	},
	{ NULL } // Sentinel
};

static int
load_hiscore_cb(void *, int count, char **values, char **colNames);

static
DbQuery GET_TOP_10_COMMAND = {
	"SELECT datetime(time, 'localtime') as time, gold, playerLevel, dungeonLevel "
		"FROM hiscore "
		"ORDER BY gold DESC "
		"LIMIT 10",
	load_hiscore_cb,
	NULL
};

static int
load_top_gold_cb(void *, int count, char **values, char **colNames);

static
DbQuery GET_TOP_GOLD_COMMAND = {
	"SELECT gold "
		"FROM hiscore "
		"ORDER BY gold DESC "
		"LIMIT 1",
	load_top_gold_cb,
	NULL
};

static sqlite3 *db = NULL;

static void
create_tables(void)
{
	for (unsigned int i = 0;; ++i) {
		DbQuery *query = &MIGRATE_COMMANDS[i];
		if (query->stmt == NULL)
			break;

		db_execute(db, query);
	}
}

void
hiscore_init(void)
{
	if (!db_open(DB_FILE, &db)) {
		db_close(&db);
		fatal("Exiting");
	}
	create_tables();
}

static void
save_hiscore(double gold, int lvl, int dlvl)
{
	const char *query = "INSERT INTO hiscore(gold, playerLevel, dungeonLevel) values (?, ?, ?)";
	sqlite3_stmt *stmt = db_prepare(db, query);

	debug("Saving high score: %dg %dpl %dl",
	      gold, lvl, dlvl);
	sqlite3_bind_double(stmt, 1, gold);
	sqlite3_bind_int(stmt, 2, lvl);
	sqlite3_bind_int(stmt, 3, dlvl);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

static void
save_hiscore_artifact(int hid, int aid)
{
	if (aid <= 0)
		return;

	const char *query = "INSERT INTO hiscore_artifacts(hiscoreId, artifactId) values(?, ?)";
	sqlite3_stmt *stmt = db_prepare(db, query);
	sqlite3_bind_int(stmt, 1, hid);
	sqlite3_bind_int(stmt, 2, aid);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}

void
hiscore_register(Player *p, unsigned int dungeonLevel)
{
	save_hiscore(p->gold, p->stats.lvl, dungeonLevel);
	int hiscoreId = (int) sqlite3_last_insert_rowid(db);
	if (!hiscoreId) {
		error("Failed to retrieve last inserted hiscore id");
		return;
	}

	for (int i = 0; i < LAST_ARTIFACT_EFFECT; ++i) {
		save_hiscore_artifact(hiscoreId, player_has_artifact(p, i) > 0 ? i : 0);
	}
}

static int
load_hiscore_cb(void *result, int count, char **values, char **colNames)
{
	HiScore *score = ec_malloc(sizeof(HiScore));
	for (int i = 0; i < count; ++i) {
		if (strcmp(colNames[i], "time") == 0) {
			long unsigned int len = strlen(values[i]) + 1;
			score->timestamp = ec_malloc(sizeof(char) * len);
			m_strcpy(score->timestamp, len, values[i]);
		}
		else if (strcmp(colNames[i], "gold") == 0)
			score->gold = atof(values[i]);
		else if (strcmp(colNames[i], "playerLevel") == 0)
			score->playerLevel = atoi(values[i]);
		else if (strcmp(colNames[i], "dungeonLevel") == 0)
			score->dungeonLevel = atoi(values[i]);
	}
	LinkedList **scores = result;
	linkedlist_append(scores, score);
	return 0;
}

LinkedList *
hiscore_get_top10(void)
{
	LinkedList *scores = linkedlist_create();
	DbQuery *query = &GET_TOP_10_COMMAND;
	query->cb_arg = &scores;
	db_execute(db, query);

	return scores;
}


static int
load_top_gold_cb(void *result, int count, char **values, char **colNames)
{
	UNUSED(count);
	UNUSED(colNames);

	double *gold = result;
	*gold = atof(values[0]);
	return 0;
}

double
hiscore_get_top_gold(void)
{
	double result;
	DbQuery *query = &GET_TOP_GOLD_COMMAND;
	query->cb_arg = &result;
	db_execute(db, query);

	return result;
}

void
hiscore_close(void)
{
	db_close(&db);
}

void
hiscore_destroy(HiScore *score)
{
	free(score->timestamp);
	free(score);
}
