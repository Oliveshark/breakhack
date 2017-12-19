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
LinkedList* linkedlist_node_create(void)
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

void linkedlist_each(LinkedList **head, void (*fun)(void*))
{
	LinkedList *next = *head;

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
