#include "n_ary_tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// N-ary Tree implementasyonu - DOM agaci

Node* create_node(const char* tag) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->tag = strdup(tag);
    node->id = NULL;
    node->class_name = NULL;
    node->parent = NULL;
    node->child_capacity = 4;
    node->child_count = 0;
    node->children = (Node**)malloc(sizeof(Node*) * node->child_capacity);
    return node;
}

void add_child(Node* parent, Node* child) {
    if (parent->child_count >= parent->child_capacity) {
        parent->child_capacity *= 2;
        parent->children = (Node**)realloc(
            parent->children,
            sizeof(Node*) * parent->child_capacity
        );
    }
    child->parent = parent;
    parent->children[parent->child_count++] = child;
}

void free_tree(Node* root) {
    if (root == NULL) return;
    for (int i = 0; i < root->child_count; i++) {
        free_tree(root->children[i]);
    }
    free(root->children);
    free(root->tag);
    if (root->id) free(root->id);
    if (root->class_name) free(root->class_name);
    free(root);
}