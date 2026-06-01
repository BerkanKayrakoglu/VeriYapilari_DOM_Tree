#ifndef PARSER_H
#define PARSER_H

#include "n_ary_tree.h"
#include "hash_table.h"


// HTML Parser - HTML metnini DOM agacina donusturur

typedef struct {
    Node* root;
    HashTable* ht;
} DOM;

DOM*  dom_parse(const char* html);
void  dom_free(DOM* dom);

// Arama fonksiyonlari
Node* dom_get_by_id(DOM* dom, const char* id);
Node** dom_get_by_class(DOM* dom, const char* class_name, int* count);

// Algoritma fonksiyonlari
int   dom_depth(Node* node);
void  dom_print(Node* root, int level);
Node** dom_bfs(DOM* dom, int* count);
Node** dom_dfs(DOM* dom, int* count);

#endif // PARSER_H