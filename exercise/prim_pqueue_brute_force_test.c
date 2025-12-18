#include <stdio.h>
#include "prim.h"
#include "prim_pqueue_brute_force.h"

int main()
{
    struct node a = {.data = 'A', .key = 3};
    struct node b = {.data = 'B', .key = 2};
    struct node c = {.data = 'C', .key = 1};

    struct pqueue *pq = new_pqueue(5);

    is_empty(pq);
    enqueue(pq, &a);

    is_empty(pq);
    enqueue(pq, &b);
    enqueue(pq, &c);

    is_node_in_pqueue(&c);
    is_node_in_pqueue(&b);
    dequeue(pq);
    is_node_in_pqueue(&b);
    dequeue(pq);
    dequeue(pq);
    is_empty(pq);

}