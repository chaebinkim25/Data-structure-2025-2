#ifndef KRUSKAL_HEADER
#define KRUSKAL_HEADER 

struct edge;

struct node {
    char data;
    int index;
    int rank;
    struct node *root;
    struct node *parent;
};

struct edge {
    struct node *node1;
    struct node *node2;
    char shape; /* -: horizontal, |: vertical, /: right angle, \: left angle */
    int type; /* 0: not checked, 1: tree edge, 2: not-used edge */
    int distance;
};

struct graph {
    struct node *vertexes;
    int n_vertexes;
    struct edge *edges;
    int n_edges;
    struct edge **sorted_edge_pointers;
    struct edge ***adj;
};

#endif

