#ifndef STACK_H
#define STACK_H
#include "n_ary_tree.h"

typedef struct StackNode {
    Node* tree_node;
    struct StackNode* next;
} StackNode;
typedef struct Stack {
    StackNode* top;
    int size;
} Stack;

Stack* create_stack();
void push(Stack* s, Node* node);
Node* pop(Stack* s);
int is_empty(Stack* s);
#endif // STACK_H
