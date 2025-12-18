#include <stdio.h>
#include <stdlib.h>
#include "kruskal.h"

void print_node(struct node *v)
{
    char str[5] = {' ', v->data, ' ', ' ', '\0'};
    str[2] = '0' + v->rank;
    if (v->root == v)
        str[3] = 'r';
    printf("%s", str);
}

void print_edge(struct edge *e)
{
    if (e != NULL) {
        char str[5] = {' ', e->shape, '0' + e->distance, ' ', '\0'};
        if (e->type == 1) {
            if (e->shape == '-') {
                str[1] = '=';
            }
            else {
                str[2] = e->shape;
                str[3] = '0' + e->distance;
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


/*
 * 3 column graph edge direction only - / | allowed.
 */
void print_graph_3col(struct graph *g)
{
    int n_vertexes = g->n_vertexes;

    for (int i = 0; i < 3 && i < n_vertexes; i++) {
        print_node(g->vertexes + i);
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


void sort_edges(struct graph *g)
{
    struct edge *edges = g->edges;
    int n = g->n_edges;

    struct edge **sorted = g->sorted_edge_pointers;

    for (int i = 0; i < n; i++) {
        struct edge *e = edges + i;
        int dist = e->distance;
        int pos = i;
        while (pos > 0) {
            if (sorted[pos - 1]->distance > dist) {
                sorted[pos] = sorted[pos - 1];
                pos -= 1;
            }
            else {
                break;
            }
        }
        sorted[pos] = e;
    }
}

static void swap(struct edge **data, int i, int j)
{
    struct edge* tmp = data[i];
    data[i] = data[j];
    data[j] = tmp;
}

void sort_edges_binary_heap(struct graph *g)
{
    struct edge *edges = g->edges;
    int n = g->n_edges;

    struct edge **sorted = g->sorted_edge_pointers;

    for (int i = 0; i < n; i++)
        sorted[i] = edges + i;

    printf("heap_sort: make_heap\n");

    for (int pos = n - 1; pos >= 0; pos--) {
        int i = pos;
        while (i * 2 + 1 < n) {
            int c = i * 2 + 1;
            if (c + 1 < n && sorted[c + 1]->distance > sorted[c]->distance) 
                c = c + 1;

            if (sorted[c]->distance > sorted[i]->distance)
                swap(sorted, i, c);
            else
                break;

            i = c;
        } 
        for (int i = 0; i < g->n_edges; i++) {
            print_edge(sorted[i]);
        }
        printf("\n");

    }


    while (n > 0) {
        printf("heap_sort: now\n");
        for (int i = 0; i < n; i++) {
            print_edge(sorted[i]);
        }
        printf("\n");
        printf("heap_sort: poping %d, %c-%c[%d], new_root: %c-%c[%d])\n", n, sorted[0]->node1->data, sorted[0]->node2->data, sorted[0]->distance, sorted[n - 1]->node1->data, sorted[n - 1]->node2->data, sorted[n - 1]->distance);
        swap(sorted, 0, n - 1);
        for (int i = 0; i < n; i++) {
            print_edge(sorted[i]);
        }
        printf("\n");
        n = n - 1;
        int i = 0;
        while (i * 2 + 1 < n) {
            int c = i * 2 + 1;
            if (c + 1 < n && sorted[c + 1]->distance > sorted[c]->distance) 
                c = c + 1;
            
            if (sorted[c]->distance > sorted[i]->distance)
                swap(sorted, i, c);
            else
                break;

            i = c;
        } 
        printf("heap sort: heap again\n");
        for (int i = 0; i < n; i++) {
            print_edge(sorted[i]);
        }
        printf("\n");

    }

    for (int i = 0; i < g->n_edges; i++) {
        print_edge(sorted[i]);
    }
    printf("\n");
}


struct node *find_root(struct node* n)
{
    if (n->root != n) {
        n->root = find_root(n->root);
    }
    return n->root;
}

void union_mst(struct node* x, struct node *y)
{
    if (x->rank < y->rank) {
        x->root = y;
        x->parent = y;
    }
    else {
        y->root = x;
        y->parent = x;

        if (x->rank == y->rank)
            x->rank += 1;            
    }
}

void kruskal(struct graph *g)
{
    sort_edges_binary_heap(g);

    struct edge **edges = g->sorted_edge_pointers;
    int n_edges = g->n_edges;

    for (int i = 0; i < n_edges; i++) {
        struct edge *e = edges[i];
        e->type = 0;
    }

    struct node *nodes = g->vertexes;
    int n_nodes = g->n_vertexes;

    for (int i = 0; i < n_nodes; i++) {
        struct node *n = nodes + i;
        n->rank = 0;
        n->root = n;
        n->parent = NULL;
    }

    for (int i = 0; i < n_edges; i++) {
        struct edge *e = edges[i];
        struct node *x = find_root(e->node1);
        struct node *y = find_root(e->node2);

        if (x == y) {
            e->type = 2;
        }
        else {
            e->type = 1;
            union_mst(x, y);
        }

        print_graph_3col(g);
    }
}

int main()
{
    printf("Hello, graph!\n");

    struct node v = {'A', .root = &v};
    print_node(&v);

    v.root = NULL;
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
        {n + 0, n + 1, '-', .distance = 0},
        {n + 1, n + 2, '-', .distance = 3},
        {n + 0, n + 3, '|', .distance = 3},
        {n + 1, n + 3, '/', .distance = 2},
        {n + 1, n + 4, '|', .distance = 2},
        {n + 2, n + 4, '/', .distance = 1},
        {n + 3, n + 4, '-', .distance = 1},
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

    struct edge *edge_pointers[7];
    g.sorted_edge_pointers = edge_pointers;

    make_adj_matrix(&g);
    print_adj_matrix(&g);

    print_graph_3col(&g);

    kruskal(&g);

    return 0;
}