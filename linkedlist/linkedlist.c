#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

static
void *linkedlist_malloc(unsigned int size)
{
	void *ptr;
	ptr = malloc(size);
	if (ptr == NULL) {
		perror("[!!] Fatal error in linkedlist_malloc() on memory allocation");
		exit(-1);
	}
	return ptr;
}

static
LinkedList* linkedlist_node_create()
{
	LinkedList *newList = linkedlist_malloc(sizeof(LinkedList));
	newList->next = NULL;
	newList->data = NULL;
	return newList;
}

LinkedList* linkedlist_create()
{
	return NULL;
}

static void copy_data(void *dest, void *src, unsigned int size)
{
	int i;
	for (i = 0; i < size; ++i)
		*(char*)(dest + i) = *(char*)(src + i);
}

/**
 * Warning! This can get a bit wonky if you append/push a complex struct that
 * contains pointers. The pointers will be copied and not duplicated in that
 * case. Be careful.
 */
void linkedlist_push(LinkedList **head, void *value, unsigned int size)
{

	LinkedList *node = linkedlist_node_create();
	node->data = linkedlist_malloc(size);
	copy_data(node->data, value, size);
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

/**
 * Warning! This can get a bit wonky if you append/push a complex struct that
 * contains pointers. The pointers will be copied and not duplicated in that
 * case. Be careful.
 */
void linkedlist_append(LinkedList **head, void *value, unsigned int size)
{
	if (*head == NULL) {
		*head = linkedlist_node_create();
		(*head)->data = linkedlist_malloc(size);
		copy_data((*head)->data, value, size);
	} else {
		linkedlist_append(&(*head)->next, value, size);
	}
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

void linkedlist_destroy(LinkedList **head)
{
	if (*head == NULL) {
		return;
	}
	if ((*head)->next != NULL) {
		linkedlist_destroy(&(*head)->next);
		free((*head)->next);
		(*head)->next = NULL;
	}

	free((*head)->data);
	(*head)->data = NULL;
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
