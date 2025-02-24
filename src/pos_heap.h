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
#ifndef _POS_HEAP_H_
#define _POS_HEAP_H_

#include "position.h"
#include <stddef.h>
#include <stdint.h>

typedef struct node {
	Position pos;
	uint32_t score;
} Node;

typedef struct PHeap {
	Node *list;
	size_t size;
	size_t capacity;
} PHeap;

/**
 * Initialize a PosHeap
 *
 * \param ph The heap to intialize
 * \param capacity The starting capacity of the heap
 */
void pheap_init(PHeap *ph, size_t capacity);

/**
 * Insert a new position into the heap
 *
 * \param ph The heap to modify
 * \param p  The value to insert
 */
void pheap_insert(PHeap *ph, Position p, uint32_t score);

/**
 * Peek the first position in the heap
 *
 * Pop the top position in the heap. Poping an empty heap is undefined
 * behaviour.
 *
 * \param ph The heap to peek
 * \returns The top element in the heap
 */
Position pheap_pop(PHeap *ph);

/**
 * Destroy a heap and free the memory
 *
 * \param ph The heap to destroy
 */
void pheap_destroy(PHeap *ph);

#endif  // _POS_HEAP_H_

