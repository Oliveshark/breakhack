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
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "hashtable.h"
#include "defines.h"
#include "util.h"

Hashtable*
ht_create(unsigned int size)
{
	Hashtable *table;
	unsigned int i;

	if (size < 1)
		return NULL;

	table = ec_malloc(sizeof(Hashtable));
	table->size = size;
	table->entries = ec_malloc((unsigned int) sizeof(Entry) * size);

	for (i = 0; i < size; ++i) {
		table->entries[i] = NULL;
	}

	return table;
}

/* D. J. Bernstein hash function */
static unsigned int
hash(Hashtable *table, const char *key)
{
	unsigned int hash = 5381;

	while (*key) {
		hash = 33 * hash ^ (unsigned char) *key++;
	}

	return hash % table->size;
}

static Entry*
entry_create(const char *key, void *value)
{
	Entry *entry;

	entry = ec_malloc(sizeof(Entry));
	entry->key = strdup(key);
	entry->value = value;
	entry->next = NULL;

	return entry;
}

void
ht_set(Hashtable *table, const char *key, void *val)
{
	unsigned int hashkey = 0;
	Entry *newEntry = NULL;
	Entry *next;
	Entry *last = NULL;

	hashkey =  hash(table, key);

	next = table->entries[hashkey];

	/* Find a position */
	while (next != NULL
	       && next->key != NULL
	       && strcmp(key, next->key) > 0)
	{
		last = next;
		next = next->next;
	}

	if (last == NULL)
		last = table->entries[hashkey];

	if (next && next->key && strcmp(key, next->key) == 0) {
		/* Collision */
		free(next->value);
		next->value = val;
	} else {
		/* New entry */
		newEntry = entry_create(key, val);

		if (next == table->entries[hashkey]) {
			table->entries[hashkey] = newEntry;
			newEntry->next = next;
		} else if(next == NULL) {
			last->next = newEntry;
		} else {
			newEntry->next = next;
			last->next = newEntry;
		}
	}
}

void*
ht_get(Hashtable *table, const char *key)
{
	unsigned int hashkey = 0;
	Entry *entry;

	hashkey = hash(table, key);

	entry = table->entries[hashkey];

	while (entry && entry->key && strcmp(entry->key, key) < 0) {
		entry = entry->next;
	}

	if (!entry || !entry->key || strcmp(entry->key, key) != 0) {
		return NULL;
	}
	return entry->value;
}

void
ht_destroy(Hashtable *table)
{
	ht_destroy_custom(table, free);
}

void
ht_destroy_custom(Hashtable *table, void (*destroy_value)(void *))
{
	Entry *entry, *next;
	unsigned int i;

	if (table == NULL) {
		return;
	}

	for (i = 0; i < table->size; ++i) {
		entry = table->entries[i];
		if (entry == NULL)
			continue;
		while (entry) {
			next = entry->next;
			destroy_value(entry->value);
			entry->value = NULL;
			free(entry->key);
			free(entry);
			entry = next;
		}
	}
	free(table->entries);
	free(table);
}
