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
#include "pos_heap.h"
#include "util.h"
#include <assert.h>

static void add_node(PHeap *ph, Position pos, uint32_t score)
{
	if (ph->size == ph->capacity) {
		ph->capacity *= 2;
		ph->list = ec_realloc(ph->list, ph->capacity * sizeof(Node));
	}
	Node *node = &ph->list[ph->size++];
	node->pos = pos;
	node->score = score;
}

static void swap(Node *a, Node *b) {
	Node tmp = *a;
	*a = *b;
	*b = tmp;
}

static void heapify(PHeap *ph, int i)
{
	if (ph->size <= 0) {
		return;
	}

	size_t size = ph->size;
	size_t smallest = i;
	size_t l = 2 * i + 1;
	size_t r = 2 * i + 2;

	if (l < size && ph->list[l].score < ph->list[smallest].score) {
		smallest = l;
	}
	if (r < size && ph->list[r].score < ph->list[smallest].score) {
		smallest = r;
	}
	if (smallest != i) {
		swap(&ph->list[i], &ph->list[smallest]);
		heapify(ph, smallest);
	}
}

void pheap_init(PHeap *ph, size_t capacity)
{
	ph->capacity = capacity > 0 ? capacity : 1;
	ph->size = 0;
	ph->list = ec_calloc(ph->capacity, sizeof(Node));
}

void pheap_insert(PHeap *ph, Position p, uint32_t score)
{
	add_node(ph, p, score);
	size_t i = ph->size - 1;
	while (i != 0 && ph->list[(i - 1)/2].score > ph->list[i].score) {
		swap(&ph->list[i], &ph->list[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
}

Position pheap_pop(PHeap *ph)
{
	Position p = ph->list[0].pos;

	if (ph->size == 1) {
		ph->size--;
	} else {
		ph->list[0] = ph->list[--ph->size];
		heapify(ph, 0);
	}
	return p;
}

void pheap_destroy(PHeap *ph)
{
	free(ph->list);
	ph->list = NULL;
}
