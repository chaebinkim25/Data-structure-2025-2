#include <stdio.h>
#include <stdlib.h>

#include "prim.h"

struct pqueue {
    struct node **data;
    int capacity;
    int size;
};

struct pqueue *new_pqueue(int capacity)
{
    struct pqueue *pq = (struct pqueue *) malloc(sizeof(struct pqueue));
    struct node **data = (struct node **) malloc(sizeof(struct node *) * capacity);

    printf("new priority queue (capacity: %d, address: %p)\n", capacity, pq);
    
    pq->data = data;
    pq->capacity = capacity;
    pq->size = 0;

    return pq;
}

void delete_pqueue(struct pqueue *pq)
{
    free(pq->data);
    free(pq);
}

void print_pqueue(struct pqueue *pq)
{
    printf("data: ");
    for (int i = 0; i < pq->size; i++) {
        printf("%c(key: %d, pq_index: %d), ", pq->data[i]->data, pq->data[i]->key, pq->data[i]->pq_index);
    }
    printf("\n");
}

void enqueue(struct pqueue *pq, struct node *n)
{
    printf("enqueue: node %c(%d)... ", n->data, n->key);

    pq->data[pq->size] = n;
    n->pq_index = pq->size;
    pq->size += 1;

    printf("pq index = %d. size of priority queue = %d\n", n->pq_index, pq->size);
    print_pqueue(pq);
}

struct node *dequeue(struct pqueue *pq)
{
    printf("dequeue: size from %d ... ", pq->size);

    if (pq->size == 0) {
        printf("size is 0, returning\n");
        return NULL;
    }

    int min_index = 0;
    int min_key = pq->data[0]->key;

    for (int i = 1; i < pq->size; i++) {
        if (pq->data[i]->key < min_key) {
            min_index = i;
            min_key = pq->data[i]->key;
        }
    }

    struct node *min_node = pq->data[min_index];
    min_node->pq_index = -1;

    for (int i = min_index; i < pq->size - 1; i++) {
        pq->data[i] = pq->data[i + 1];
        pq->data[i]->pq_index = i;
    }
    pq->size -= 1;

    printf("to %d. returning node: %c(%d)\n", pq->size, min_node->data, min_node->key);
    print_pqueue(pq);

    return min_node;
}

void decrease_key(struct pqueue *pq, struct node *n)
{

}

int is_node_in_pqueue(struct node *n)
{
    if (n->pq_index >= 0) {
        printf("node %c is in a priority queue\n", n->data);
        return 1;
    }
    else {
        printf("node %c is not in a priority queue\n", n->data);
        return 0;
    }
}

int is_empty(struct pqueue *pq) 
{
    if (pq->size > 0) {
        printf("pq is not empty\n");
        return 0;
    }
    else {
        printf("pq is empty\n");
        return 1;
    }
}