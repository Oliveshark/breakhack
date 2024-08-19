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

#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "util.h"

static
LinkedList* linkedlist_node_create(void)
{
	LinkedList *newList = ec_malloc(sizeof(LinkedList));
	newList->next = NULL;
	newList->data = NULL;
	return newList;
}

LinkedList* linkedlist_create(void)
{
	return NULL;
}

void linkedlist_push(LinkedList **head, void *value)
{

	LinkedList *node = linkedlist_node_create();
	node->data = value;
	node->next = *head;

	*head = node;
}

void* linkedlist_pop(LinkedList **head)
{
	if (*head == NULL)
		return NULL;

	void *data = (*head)->data;
	LinkedList *oldHead = *head;
	*head = (*head)->next;
	free(oldHead);

	return data;
}

void linkedlist_append(LinkedList **head, void *value)
{
	LinkedList *node;

	if (*head == NULL) {
		*head = linkedlist_node_create();
		(*head)->data = value;
		return;
	}

	node = *head;
	while (node->next != NULL)
		node = node->next;

	node->next = linkedlist_node_create();
	node->next->data = value;
}

void* linkedlist_poplast(LinkedList **head)
{
	if (*head == NULL)
		return NULL;

	if ((*head)->next == NULL) {
		void* data = (*head)->data;
		free(*head);
		*head = NULL;
		return data;
	}

	LinkedList *nextNode = (*head)->next;
	if (nextNode->next == NULL) {
		void *data = nextNode->data;
		free((*head)->next);
		(*head)->next = NULL;
		return data;
	}

	return linkedlist_poplast(&nextNode);
}

void* linkedlist_get(LinkedList **head, unsigned int index)
{
	if (*head == NULL)
		return NULL;
	if (index == 0)
		return (*head)->data;

	return linkedlist_get(&(*head)->next, --index);
}

/* cppcheck-suppress constParameter */
void linkedlist_each(LinkedList **head, void (*fun)(void*))
{
	const LinkedList *next = *head;

	while (next != NULL) {
		fun(next->data);
		next = next->next;
	}
}

void linkedlist_destroy(LinkedList **head)
{
	if (*head == NULL) {
		return;
	}

	linkedlist_destroy(&(*head)->next);

	if ((*head)->data != NULL) {
		free((*head)->data);
		(*head)->data = NULL;
	}
	free(*head);
	*head = NULL;
}

unsigned int linkedlist_size(LinkedList *head)
{
	if (head == NULL) {
		return 0;
	}
	return 1 + linkedlist_size(head->next);
}
