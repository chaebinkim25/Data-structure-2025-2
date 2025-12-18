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

void swap(struct pqueue *pq, int i, int j)
{
    struct node** data = pq->data;
    struct node* tmp = data[i];
    data[i] = data[j];
    data[j] = tmp;

    data[i]->pq_index = i;
    data[j]->pq_index = j;
}

void enqueue(struct pqueue *pq, struct node *n)
{
    printf("enqueue: node %c(%d)... ", n->data, n->key);
    int i = pq->size;
    struct node **data = pq->data;

    data[i] = n;
    n->pq_index = i;
    pq->size += 1;

    while (i > 0) {
        int p = (i - 1) / 2;
        if (data[i]->key < data[p]->key)
            swap(pq, i, p);
        else
            break;
        i = p;
    }

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

    struct node **data = pq->data;
    int size = pq->size - 1;

    struct node *min_node = data[0];
    swap(pq, 0, size);
    min_node->pq_index = -1;

    pq->size = size;

    int i = 0;
    while (i * 2 + 1 < size) {
        int c = i * 2 + 1;
        if (c + 1 < size && data[c + 1]->key < data[c]->key) {
            c = c + 1;
        }

        if (data[i]->key > data[c]->key) 
            swap(pq, i, c);
        else 
            break;
        
        i = c;
    }

    printf("to %d. returning node: %c(%d)\n", pq->size, min_node->data, min_node->key);
    print_pqueue(pq);

    return min_node;
}

void decrease_key(struct pqueue *pq, struct node *n)
{
    struct node **data = pq->data;
    int i = n->pq_index;

    while (i > 0) {
        int p = (i - 1) / 2;
        if (data[i]->key < data[p]->key)
            swap(pq, i, p);
        else
            break;
        i = p;
    }
    
    printf("decrease-key: ");
    print_pqueue(pq);    
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