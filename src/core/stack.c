#include "stack.h"
#include <stdio.h>


// Stack implementasyonu

void stack_init(Stack* s) {
    s->top = -1;
}

int stack_is_empty(Stack* s) {
    return s->top == -1;
}

void stack_push(Stack* s, Node* node) {
    if (s->top >= STACK_MAX - 1) {
        printf("Stack dolu!\n");
        return;
    }
    s->data[++(s->top)] = node;
}

Node* stack_pop(Stack* s) {
    if (stack_is_empty(s)) {
        printf("Stack bos!\n");
        return NULL;
    }
    return s->data[(s->top)--];
}

Node* stack_peek(Stack* s) {
    if (stack_is_empty(s)) return NULL;
    return s->data[s->top];
}