#include "queue.h"
#include <stdio.h>


// Queue implementasyonu

void queue_init(Queue* q) {
    q->front = 0;
    q->rear = 0;
    q->size = 0;
}

int queue_is_empty(Queue* q) {
    return q->size == 0;
}

void queue_enqueue(Queue* q, Node* node) {
    if (q->size >= QUEUE_MAX) {
        printf("Queue dolu!\n");
        return;
    }
    q->data[q->rear] = node;
    q->rear = (q->rear + 1) % QUEUE_MAX;
    q->size++;
}

Node* queue_dequeue(Queue* q) {
    if (queue_is_empty(q)) {
        printf("Queue bos!\n");
        return NULL;
    }
    Node* node = q->data[q->front];
    q->front = (q->front + 1) % QUEUE_MAX;
    q->size--;
    return node;
}