#ifndef QUEUE_H
#define QUEUE_H

#include "n_ary_tree.h"

// Queue (Kuyruk) - BFS icin kullanilir

#define QUEUE_MAX 1000

typedef struct {
    Node* data[QUEUE_MAX];
    int front;
    int rear;
    int size;
} Queue;

void queue_init(Queue* q);
int  queue_is_empty(Queue* q);
void queue_enqueue(Queue* q, Node* node);
Node* queue_dequeue(Queue* q);

#endif // QUEUE_H