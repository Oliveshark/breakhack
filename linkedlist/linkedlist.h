#ifndef LINKEDLIST_H_
#define	LINKEDLIST_H_

struct Node {
	struct Node *next;
	void *data;
};
typedef struct Node LinkedList;

LinkedList* linkedlist_create();

void linkedlist_push(LinkedList **head, void *value, size_t size);

void* linkedlist_pop(LinkedList **head);

void linkedlist_append(LinkedList **head, void *value, size_t size);

void* linkedlist_poplast(LinkedList **head);

void linkedlist_destroy(LinkedList **head);

unsigned int linkedlist_size(LinkedList *head);
	

#endif // LINKEDLIST_H_
