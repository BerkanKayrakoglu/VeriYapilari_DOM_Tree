#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "n_ary_tree.h"

// Hash Table - getElementById icin O(1) erisim

#define TABLE_SIZE 256

typedef struct HashEntry {
    char* key;
    Node* value;
    struct HashEntry* next;
} HashEntry;

typedef struct {
    HashEntry* buckets[TABLE_SIZE];
} HashTable;

void  hash_init(HashTable* ht);
void  hash_insert(HashTable* ht, const char* key, Node* node);
Node* hash_get(HashTable* ht, const char* key);
void  hash_free(HashTable* ht);

#endif // HASH_TABLE_H