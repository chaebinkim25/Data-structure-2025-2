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
	int status;		/* 0: 처리전, 1: 처리후 */
	int distance;
	struct node* parent;
	struct elist *adj;
};

struct edge {
	struct node *node1;
	struct node *node2;
	char shape;			/* '-', '/', '|' */
	int length;			/* 연결선의 길이 */
};

struct graph {
	struct node *vertexes;
	int n_vertexes;
	struct edge *edges;
	int n_edges;
	struct edge ***adj;
};

void print_node(struct node *v)
{
	char str[4] = "   ";
	str[0] = v->data;
	str[1] = '0' + v->distance;
	if (v->status != 0)
		str[2] = '*';

	printf("%s", str);
}

void print_edge(struct edge *e)
{
	char str[4] = "   ";
	if (e != NULL) {
		str[0] = '0' + e->length;
		str[1] = e->shape;
	}
	printf("%s", str);
}

void print_graph(struct graph *g)
{
	/* 노드 출력 */
	for (int i = 0; i < g->n_vertexes; i++) {
		print_node(g->vertexes + i);
		printf(", ");
	}
	printf("\n");
	/* 연결선 출력 */
	for (int i = 0; i < g->n_edges; i++) {
		printf("[");
		print_node(g->edges[i].node1);
		print_edge(g->edges + i);
		print_node(g->edges[i].node2);
		printf("], ");
	}
	printf("\n");
}

struct elist *new_elist(struct edge *e, struct elist *n)
{
	struct elist *el = malloc(sizeof(struct elist));
	if (el == NULL) return NULL;
	el->edge = e;
	el->next = n;
	return el;
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
	/* 첫번째 행: 노드 출력 */
	printf("  ");
	for (int i = 0; i < g->n_vertexes; i++) {
		printf("%c ", g->vertexes[i].data);
	}
	printf("\n");
	for (int i = 0; i < g->n_vertexes; i++) {
		/* 첫번째 열: 노드 출력 */
		printf("%c ", g->vertexes[i].data);
		for (int j = 0; j < g->n_vertexes; j++) {
			if (g->adj[i][j] == NULL) 
					printf("  ");
			else
					printf("%c ", g->adj[i][j]->shape);	
		}
		printf("\n");
	}
}


void make_adj_list(struct graph *g)
{
	for (int i = g->n_edges - 1; i >= 0; i--) {
		struct edge *e = g->edges + i;
		struct node *u = e->node1;
		struct node *v = e->node2;
		struct elist *eu = new_elist(e, u->adj);
		if (eu != NULL)
			u->adj = eu;
		struct elist *ev = new_elist(e, v->adj);
		if (ev != NULL)
			v->adj = ev;
	}
}

void free_adj_list(struct graph *g)
{
	for (int i = 0; i < g->n_vertexes; i++) {
		while (g->vertexes[i].adj != NULL) {
			struct elist *l = g->vertexes[i].adj;
			g->vertexes[i].adj = l->next;
			free(l);
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
			struct node *near = e->edge->node1;
			if (near == n) {
				near = e->edge->node2;
			}
			printf("%c ", near->data);
			e = e->next;
		}
		printf("\n");
	}
}

		
void print_graph_3col(struct graph *g)
{
	int n_vertexes = g->n_vertexes;

	/* 첫번째 행 */
	for (int i = 0; i < 3 && i < n_vertexes; i++) {
		print_node(g->vertexes + i);
		print_edge(g->adj[i][i + 1]);
	}
	printf("\n");
	for (int row = 1; row <= n_vertexes / 3; row++) {
		int i;
		for (i = row * 3; i < row * 3 + 2 
				&& i < n_vertexes; i++) {
			print_edge(g->adj[i][i - 3]);
			print_edge(g->adj[i][i - 2]);
		}
		if (i < n_vertexes)
			print_edge(g->adj[i][i - 3]);
		printf("\n");
		for (i = row * 3; i <= row * 3 + 2 
				&& i < n_vertexes; i++) {
			print_node(g->vertexes + i);
			print_edge(g->adj[i][i + 1]);
		}
		printf("\n");
	}
}

struct queue {
	struct node **data;
	int front;
	int rear;
};

struct queue *new_queue(int size)
{
	struct queue *q = malloc(sizeof(struct queue));
	q->data = malloc(sizeof(struct node*) * size);
	q->front = 0;
	q->rear = 0;    
	return q;
}

void enqueue(struct queue *q, struct node *n)
{
	q->data[q->rear++] = n;
}

struct node *dequeue(struct queue *q)
{
	return q->data[q->front++];
}

int is_empty(struct queue *q)
{
	return q->front == q->rear;
}

void delete_queue(struct queue *q)
{
	free(q->data);
	free(q);
}

void bfs(struct graph *g, struct node *s)
{
	for (int i = 0; i < g->n_vertexes; i++) {
		struct node *u = g->vertexes + i;
		u->distance = 9;
		u->parent = NULL;
		u->status = 0;
	}
	
	print_graph_3col(g);

	struct queue *to_visit = new_queue(g->n_vertexes);
	s->distance = 0;
	s->status = 1;
	enqueue(to_visit, s);
	print_graph_3col(g);
	
	while (!is_empty(to_visit)) {
		struct node *u = dequeue(to_visit);
		struct elist *near = u->adj;
		while (near != NULL) {

			struct node *v;

			if (near->edge->node1 == u)
				v = near->edge->node2;
			else
				v = near->edge->node1;

			if (v->status == 0) {
				v->parent = u;
				v->distance = u->distance + 1;
				v->status = 1;
				enqueue(to_visit, v);
				print_graph_3col(g);
			}
			near = near->next;
		}
	}

	delete_queue(to_visit);
}

struct deque_node {
	struct node *data;
	struct deque_node *prev, *next;
};

struct deque {
	struct deque_node *head;
};

struct deque_node *new_deque_node(struct node *n)
{
	struct deque_node *dnode = malloc(sizeof(struct deque_node));
	dnode->data = n;
	dnode->prev = dnode;
	dnode->next = dnode;
	return dnode;
}

struct deque *new_deque(int size)
{
	struct deque *q = malloc(sizeof(struct deque));
	if (q == NULL) return NULL;

	struct deque_node *dummy = new_deque_node(NULL);
	if (dummy == NULL) {
		free(q);
		return NULL;
	}

	q->head = dummy;
	return q;
}

int push_front(struct deque *q, struct node *n)
{
	struct deque_node *new_node = new_deque_node(n);
	if (new_node == NULL) return -1;

	new_node->prev = q->head;
	new_node->next = q->head->next;
	q->head->next->prev = new_node;
	q->head->next = new_node;

	return 0;
}

int push_rear(struct deque *q, struct node *n)
{
	struct deque_node *new_node = new_deque_node(n);
	if (new_node == NULL) return -1;

	new_node->next = q->head;
	new_node->prev = q->head->prev;
	q->head->prev->next = new_node;
	q->head->prev = new_node;

	return 0;
}

struct node *pop_front(struct deque *q)
{
	if (q->head->next == q->head) return NULL;

	struct deque_node *old_front = q->head->next;
	struct node *n = old_front->data;

	old_front->next->prev = q->head;
	q->head->next = old_front->next;

	free(old_front);
	return n;
}

struct node *pop_rear(struct deque *q)
{
	if (q->head->prev == q->head) return NULL;

	struct deque_node *old_rear = q->head->prev;
	struct node *n = old_rear->data;

	old_rear->prev->next = q->head;
	q->head->prev = old_rear->prev;

	free(old_rear);
	return n;
}

int deque_is_empty(struct deque *q)
{
	return q->head->next == q->head;
}

void delete_deque(struct deque *q)
{
	struct deque_node *n = q->head->next;
	while (n != q->head) {
		struct deque_node *old = n;
		n = n->next;
		free(old);
	}
	free(q->head);
	free(q);
}

void bfs_0_1(struct graph *g, struct node *s)
{
	for (int i = 0; i < g->n_vertexes; i++) {
		struct node *u = g->vertexes + i;
		u->distance = 9;
		u->parent = NULL;
		u->status = 0;
	}
	struct deque *q = new_deque(g->n_vertexes);
	print_graph_3col(g);
	s->distance = 0;
	push_front(q, s);
	print_graph_3col(g);	

	while (!deque_is_empty(q)) {
		struct node *u = pop_front(q);
		u->status = 1;
		print_graph_3col(g);	

		struct elist *near = u->adj;
		while (near != NULL) {
			struct edge *e = near->edge;
			int dist = u->distance + e->length;
			struct node *v;
			if (e->node1 == u)
				v = e->node2;
			else
				v = e->node1;
			if (v->distance > dist) {
				v->parent = u;
				v->distance = dist;
				if (e->length == 0)
					push_front(q, v);
				else
					push_rear(q, v);
				print_graph_3col(g);
			}
			near = near->next;
		}
	}
	delete_deque(q);
}

struct pqueue {
	struct node **data;
	int *key;
	int size;
};

struct pqueue *new_pqueue(int size)
{
	struct pqueue *pq = malloc(sizeof(struct pqueue));
	if (pq == NULL) return NULL;

	pq->data = malloc(sizeof(struct node *) * size);
	pq->key = malloc(sizeof(int) * size);
	pq->size = 0;

	return pq;
}

void swap(struct pqueue *pq, int i, int j)
{
	struct node **data = pq->data;
	int *key = pq->key;

	int tmp = key[i];
	struct node *tmp_node = data[i];
	key[i] = key[j];
	data[i] = data[j];
	key[j] = tmp;
	data[j] = tmp_node;
}

void push(struct pqueue *pq, struct node *n)
{
	int i = pq->size;
	struct node **data = pq->data;
	int *key = pq->key;

	data[i] = n;
	key[i] = n->distance;

	pq->size = i + 1;

	while (i > 0) {
		int p = (i - 1) / 2;
		if (key[i] >= key[p])
			break;
		swap(pq, i, p);
		i = p;
	}

	printf("push: node %c[%d], size = %d\n", n->data, n->distance, pq->size);
}

struct node *pop(struct pqueue *pq)
{
	struct node **data = pq->data;
	int *key = pq->key;
	int size = pq->size;

	struct node *result = data[0];
	int distance = key[0];

	swap(pq, 0, size - 1);
	pq->size = size = size - 1;

	int i = 0;

	while (2 * i + 1 < size) {
		int c = 2 * i + 1;
		if (c + 1 < size && key[c + 1] < key[c])
			c = c + 1;
		if (key[i] <= key[c])
			break;		
		swap (pq, i, c);
		i = c;
	}

	printf("pop: node %c[%d], size = %d\n", result->data, distance, pq->size);	
	return result;
}

int pqueue_is_empty(struct pqueue *pq)
{
	return pq->size == 0;
}

void delete_pqueue(struct pqueue *pq)
{
	free(pq->data);
	free(pq->key);
	free(pq);
}

void dijkstra(struct graph *g, struct node *s)
{
	for (int i = 0; i < g->n_vertexes; i++) {
		struct node *u = g->vertexes + i;
		u->distance = 9;
		u->parent = NULL;
		u->status = 0;
	}
	struct pqueue *q = new_pqueue(g->n_edges);
	print_graph_3col(g);
	s->distance = 0;
	push(q, s);
	print_graph_3col(g);	

	while (!pqueue_is_empty(q)) {
		struct node *u = pop(q);
		if (u->status == 1) 
			continue;
		u->status = 1;
		print_graph_3col(g);	
		struct elist *near = u->adj;
		while (near != NULL) {
			struct edge *e = near->edge;
			int dist = u->distance + e->length;
			struct node *v;
			if (e->node1 == u)
				v = e->node2;
			else
				v = e->node1;
			if (v->distance > dist) {
				v->parent = u;
				v->distance = dist;
				push(q, v);
				print_graph_3col(g);
			}
			near = near->next;
		}
	}
	delete_pqueue(q);
}



int main()
{
	printf("Hello, shortest path!\n");

	struct node v = {'A'};
	print_node(&v);
	v.status = 1;
	print_node(&v);
	v.distance = 2;
	print_node(&v);

	struct edge e = {NULL, NULL, '-', 0};
	print_edge(&e);
	e.length = 1;
	print_edge(&e);
	e.length = 2;
	print_edge(&e);	

	struct graph g;
	struct node n[5] = {
		{'A', 0}, {'B', 1}, {'C', 2}, {'D', 3}, {'E', 4}
	};
	g.vertexes = n;
	g.n_vertexes = 5;

	struct edge edges[] = {
		{n + 0, n + 1, '-', 1},
		{n + 1, n + 2, '-', 1},
		{n + 0, n + 3, '|', 1},
		{n + 1, n + 3, '/', 1},
		{n + 1, n + 4, '|', 1},
		{n + 2, n + 4, '/', 1}
	};
	g.edges = edges;
	g.n_edges = 6;

	print_graph(&g);

	struct edge *mat[6][6] = { 0 };
	struct edge **rows[6] = {mat[0], mat[1], mat[2],
		mat[3], mat[4], mat[5]};
	g.adj = rows;
	make_adj_matrix(&g);
	print_adj_matrix(&g);

	make_adj_list(&g);
	print_adj_list(&g);

	print_graph_3col(&g);

	bfs(&g, g.vertexes + 0);


	g.edges[0].length = 0;
	g.edges[3].length = 0;

	bfs_0_1(&g, g.vertexes + 0);

	g.edges[4].length = 3;
	dijkstra(&g, g.vertexes + 0);

	free_adj_list(&g);

	return 0;
}