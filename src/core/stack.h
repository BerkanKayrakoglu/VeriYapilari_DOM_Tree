#ifndef STACK_H
#define STACK_H

#include "n_ary_tree.h"




#define STACK_MAX 1000

typedef struct {
    Node* data[STACK_MAX];
    int top;
} Stack;

// Fonksiyon prototipleri
void stack_init(Stack* s);
int  stack_is_empty(Stack* s);
void stack_push(Stack* s, Node* node);
Node* stack_pop(Stack* s);
Node* stack_peek(Stack* s);

#endif // STACK_H