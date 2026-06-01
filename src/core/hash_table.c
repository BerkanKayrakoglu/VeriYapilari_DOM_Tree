#include "hash_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Hash Table implementasyonu

static unsigned int hash_func(const char* key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key;
        key++;
    }
    return hash % TABLE_SIZE;
}

void hash_init(HashTable* ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht->buckets[i] = NULL;
    }
}

void hash_insert(HashTable* ht, const char* key, Node* node) {
    unsigned int index = hash_func(key);
    HashEntry* entry = (HashEntry*)malloc(sizeof(HashEntry));
    entry->key = strdup(key);
    entry->value = node;
    entry->next = ht->buckets[index];
    ht->buckets[index] = entry;
}

Node* hash_get(HashTable* ht, const char* key) {
    unsigned int index = hash_func(key);
    HashEntry* entry = ht->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void hash_free(HashTable* ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashEntry* entry = ht->buckets[i];
        while (entry) {
            HashEntry* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
        ht->buckets[i] = NULL;
    }
}