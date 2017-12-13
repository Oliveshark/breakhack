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
