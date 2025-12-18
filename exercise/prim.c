#include <stdio.h>
#include <stdlib.h>
#include "prim.h"
#include "prim_pqueue_binary_heap.h"

void print_node(struct node *v)
{
    char str[5] = {' ', v->data, ' ', ' ', '\0'};
    if (v->status == 1)
        str[2] = 'o';
    printf("%s", str);
}

void print_edge(struct edge *e)
{
    if (e != NULL) {
        char str[5] = {' ', e->shape, '0' + e->length, ' ', '\0'};
        if (e->type == 1) {
            if (e->shape == '-') {
                str[1] = '=';
            }
            else {
                str[2] = e->shape;
                str[3] = '0' + e->length;
            }
        }
        else if (e->type == 2) {
            str[1] = '.';
            str[2] = ' ';
            str[3] = ' ';
        }
        printf("%s", str);
    }
    else {
        printf("   ");
    }
}


void print_graph(struct graph *g)
{
    for (int i = 0; i < g->n_vertexes; i++) {
        print_node(g->vertexes + i);
        printf(", ");
    }
    printf("\n");
    for (int i = 0; i < g->n_edges; i++) {
        printf("[");
        print_node(g->edges[i].node1);
        print_edge(g->edges + i);
        print_node(g->edges[i].node2);
        printf("], ");
    }
    printf("\n");
}

void make_adj_matrix(struct graph *g)
{
    for (int i = 0; i < g->n_edges; i++) {
        struct edge *e = g->edges + i;
        int n1 = e->node1->index;
        int n2 = e->node2->index;
        g->adj[n1][n2] = g->adj[n2][n1] = e;
    }
}

void print_adj_matrix(struct graph *g)
{
    printf("  ");
    for (int i = 0; i < g->n_vertexes; i++) {
        printf("%c ", g->vertexes[i].data);
    }
    printf("\n");

    for (int i = 0; i < g->n_vertexes; i++) {
        printf("%c ", g->vertexes[i].data);
        for (int j = 0; j < g->n_vertexes; j++) {
            if (g->adj[i][j] == NULL) {
                printf("  ");
            }
            else {
                printf("%c ", g->adj[i][j]->shape);
            }
        }
        printf("\n");
    }
}

void make_adj_list(struct graph *g)
{
    for (int i = 0; i < g->n_vertexes; i++) {
        g->vertexes[i].adj = NULL;
        for (int j = g->n_vertexes - 1; j >= 0; j--) {
            if (g->adj[i][j] != NULL) {
                struct elist *l = malloc(sizeof(struct elist));
                l->edge = g->adj[i][j];
                l->next = g->vertexes[i].adj;
                g->vertexes[i].adj = l;
            }
        }
    }
}

void free_adj_list(struct graph *g)
{
    for (int i = 0; i < g->n_vertexes; i++) {
        struct node *n = g->vertexes + i;
        struct elist *l = n->adj;
        while (l != NULL) {
            struct elist *old = l;
            l = l->next;
            free(old);
        }
    }
}

void print_adj_list(struct graph *g)
{
    for (int i = 0; i < g->n_vertexes; i++) {
        struct node *n = g->vertexes + i;
        printf("%c: ", n->data);
        struct elist *e = n->adj;
        while (e != NULL) {
            struct node *neighbor = e->edge->node1;
            if (neighbor == n) {
                neighbor = e->edge->node2;
            }
            printf("%c ", neighbor->data);
            e = e->next;
        }
        printf("\n");
    }
}

/*
 * 3 column graph edge direction only - / | allowed.
 */
void print_graph_3col(struct graph *g)
{
    int n_vertexes = g->n_vertexes;

    for (int i = 0; i < 3 && i < n_vertexes; i++) {
        print_node(g->vertexes + i);
        if (i + 1 < n_vertexes)
            print_edge(g->adj[i][i + 1]);        
    }
    printf("\n");

    for (int row = 1; row <= n_vertexes / 3; row++) {
        int i;

        for (i = row * 3; i < row * 3 + 2 && i < n_vertexes; i++) {
            print_edge(g->adj[i][i - 3]);
            print_edge(g->adj[i][i - 2]);
        }

        if (i < n_vertexes)
            print_edge(g->adj[row * 3 + 2][row * 3 - 1]);

        printf("\n");

        for (int i = row * 3; i <= row * 3 + 2 && i < n_vertexes; i++) {
            print_node(g->vertexes + i);
            if (i + 1 < n_vertexes)
                print_edge(g->adj[i][i + 1]);
        }
        printf("\n");
    }
}

struct node *get_another_node(struct edge *e, struct node *n)
{
    if (e->node1 == n)
        return e->node2;
    else
        return e->node1;
}

void prim(struct graph *g, struct node *start_node)
{
    if (start_node == NULL) {
        printf("Error: No start node.\n");
        return;
    }

    if (g == NULL) {
        printf("Error: No graph. \n");
        return;
    }

    struct edge *edges = g->edges;
    int n_edges = g->n_edges;

    for (int i = 0; i < n_edges; i++) {
        struct edge *e = edges + i;
        e->type = 0;
    }

    struct node *nodes = g->vertexes;
    int n_nodes = g->n_vertexes;

    for (int i = 0; i < n_nodes; i++) {
        struct node *n = nodes + i;
        n->status = 0;
        n->key = 10000;
        n->tree_edge = NULL;
        n->pq_index = -1;
    }

    start_node->key = 0;
    
    struct pqueue *pq = new_pqueue(n_nodes);
    enqueue(pq, start_node);

    while (is_empty(pq) == 0) {
        struct node *u = dequeue(pq);
        if (u->status != 0) 
            continue;

        printf("prim: node %c is added to mst\n", u->data);
        u->status = 1;

        struct elist *adj = u->adj;

        while (adj != NULL) {
            struct edge *e = adj->edge;
            struct node *v = get_another_node(e, u);

            printf("prim: checking node %c\n", v->data);

            if (v->status != 0 || e->type > 0) {
                adj = adj->next;
                continue;
            }
            
            if (!is_node_in_pqueue(v)) {
                v->key = e->length;
                v->tree_edge = e;
                e->type = 1;

                enqueue(pq, v);
            }
            else {
                if (v->key > e->length) {
                    v->key = e->length;

                    v->tree_edge->type = 2;
                    v->tree_edge = e;
                    e->type = 1;

                    decrease_key(pq, v);
                }                
                else {
                    e->type = 2;
                }
            }

            print_graph_3col(g);

            adj = adj->next;
        }
    }

    print_graph_3col(g);
}

int main()
{
    printf("Hello, graph!\n");

    struct node v = {'A'};
    print_node(&v);

    v.status = 1;
    print_node(&v);

    v.status = 2;
    print_node(&v);

    printf("\n");

    struct edge e = {NULL, NULL, '-', 0};
    print_edge(&e);

    e.type = 1;
    print_edge(&e);

    e.type = 2;
    print_edge(&e);

    printf("\n");

    struct graph g;
    struct node nodes[2] = {{.data = 'A'}, {.data = 'B'}};
    g.vertexes = nodes;
    g.n_vertexes = 2;
    struct edge edges[1] = {{.node1 = g.vertexes + 0, .node2 = g.vertexes + 1, .shape = '-'}};
    g.edges = edges;
    g.n_edges = 1;

    print_graph(&g);

    struct node n[5] = {
        {'A', 0}, 
        {'B', 1}, 
        {'C', 2}, 
        {'D', 3}, 
        {'E', 4}
    };

    g.vertexes = n;
    g.n_vertexes = 5;

    struct edge edges2[] = {
        {n + 0, n + 1, '-', .length = 0},
        {n + 1, n + 2, '-', .length = 3},
        {n + 0, n + 3, '|', .length = 3},
        {n + 1, n + 3, '/', .length = 2},
        {n + 1, n + 4, '|', .length = 2},
        {n + 2, n + 4, '/', .length = 1},
        {n + 3, n + 4, '-', .length = 1},
    };

    g.edges = edges2;
    g.n_edges = 7;

    struct edge *adj_matrix[6][6] = {0};
    struct edge **adj_rows[6] = {
        adj_matrix[0],
        adj_matrix[1],
        adj_matrix[2],
        adj_matrix[3],
        adj_matrix[4],
        adj_matrix[5]
    };

    g.adj = adj_rows;

    make_adj_matrix(&g);
    print_adj_matrix(&g);

    make_adj_list(&g);
    print_adj_list(&g);

    print_graph_3col(&g);

    prim(&g, n + 0);

    free_adj_list(&g);
    return 0;
}