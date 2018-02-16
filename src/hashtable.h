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

#ifndef HASHTABLE_H_
#define	HASHTABLE_H_

typedef struct entry_t {
	char *key;
	void *value;
	struct entry_t *next;
} Entry;

typedef struct table_t {
	unsigned int size;
	Entry **entries;
} Hashtable;

Hashtable* ht_create(unsigned int size);

void ht_set(Hashtable*, const char *key, void *val);

void* ht_get(Hashtable*, const char *key);

void ht_destroy(Hashtable*);

void ht_destroy_custom(Hashtable*, void (*destroy_value)(void*));

#endif // HASHTABLE_H_
