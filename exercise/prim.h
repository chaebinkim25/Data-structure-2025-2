#ifndef PRIM_HEADER

#define PRIM_HEADER 

struct edge;

struct elist {
    struct edge *edge;
    struct elist *next;
};

struct node {
    char data;
    int index;
    int status; /* 0: not in MST, 1: in MST */
    int key;
    int pq_index;
    struct elist *adj;
    struct edge *tree_edge;
};

struct edge {
    struct node *node1;
    struct node *node2;
    char shape; /* -: horizontal, |: vertical, /: right angle, \: left angle */
    int type; /* 0: not checked, 1: tree edge */
    int length;
};

struct graph {
    struct node *vertexes;
    int n_vertexes;
    struct edge *edges;
    int n_edges;
    struct edge ***adj;
};

#endif

