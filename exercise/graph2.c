#include <stdio.h>
#include <stdlib.h>

struct edge;

struct elist {
    struct edge *edge;
    struct elist *next;
};

struct node {
    char data;
    int index;
    int status; /* 0: not visited, 1: being visted, 2: visited */
    struct elist *adj;
    int group;
};

struct edge {
    struct node *node1;
    struct node *node2;
    char shape; /* -: horizontal, |: vertical, /: diagonal */
    int dfs_type; /* 0: not checked, 1: tree edge, 2: back edge */
};

void print_node(struct node *v)
{
    char str[5] = {' ', v->data, ' ', ' ', '\0'};
    if (v->status != 0)
        str[0] = '(';
    if (v->status == 2)
        str[3] = ')';
    if (v->group > 0)
        str[2] = '0' + v->group;
    printf("%s", str);
}

void print_edge(struct edge *e)
{
    if (e != NULL) {
        char str[5] = {' ', e->shape, ' ', ' ', '\0'};
        if (e->dfs_type == 2) {
            str[1] = '.';
        } else if (e->dfs_type == 1) {
            if (e->shape == '-')
                str[1] = '=';
            else
                str[2] = e->shape;
        }
        printf("%s", str);
    }
    else {
        printf("   ");
    }
}

struct graph {
    struct node *vertexes;
    int n_vertexes;
    struct edge *edges;
    int n_edges;
    struct edge ***adj;
};

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

void dfs_prep(struct graph *g)
{
    for (int i = 0; i < g->n_vertexes; i++) {
        g->vertexes[i].status = 0;
    }

    for (int j = 0; j < g->n_edges; j++) {
        g->edges[j].dfs_type = 0;
    }
}

int group = 0;

void dfs(struct graph *g, struct node *n, struct node *p)
{
    if (p == NULL) {
        printf("dfs start: %c \n", n->data);
    }
    if (n != NULL && n->status == 0) {

        n->status = 1;
        n->group = group;
        print_graph_3col(g);

        struct elist *e = n->adj;
        while (e != NULL) {

            struct node *neighbor = e->edge->node1;
            if (neighbor == n)
                neighbor = e->edge->node2;

            printf("dfs: %c and %c \n", n->data, neighbor->data);

            if (neighbor == p || e->edge->dfs_type != 0) {
                printf("continue...\n");
                e = e->next;
                continue;
            }
                
            if (neighbor->status == 0) {
                e->edge->dfs_type = 1;
                print_graph_3col(g);
                dfs(g, neighbor, n);
            }
            else if (neighbor->status == 1) {
                e->edge->dfs_type = 2;
                print_graph_3col(g);
            }
            else {
                printf("Error: %c and %c \n", n->data, neighbor->data);
            }
            e = e->next;
        }

        printf("node done\n");

        n->status = 2;
        print_graph_3col(g);
    }
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

    e.dfs_type = 1;
    print_edge(&e);

    e.dfs_type = 2;
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
        {n + 0, n + 1, '-'},
        {n + 0, n + 3, '|'},
        {n + 1, n + 3, '/'},
        {n + 2, n + 4, '/'},
    };

    g.edges = edges2;
    g.n_edges = 4;

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

    dfs_prep(&g);
    for (int i = 0; i < g.n_vertexes; i++) {
        if (g.vertexes[i].group == 0) {
            group += 1;
            dfs(&g, g.vertexes + i, NULL);
        }
    }
    return 0;
}