#include <stdio.h>
#include <stdlib.h>

/* A point in the maze */
struct point {
        int row;
        int col;
        int prev_size;  // how many points in the path before this point
};

/* Get the status of a maze cell */
int maze_status(int** maze, struct point point)
{
        return maze[point.row][point.col];
}

/* Mark a maze cell as checked */
void maze_mark(int** maze, struct point point)
{
        maze[point.row][point.col] = 1;
}


/*                 -- stack of points (array list implementation) */

/* A stack of points implemented as an array list */
struct alstack {
        struct point* points;
        int size;
        int capacity;
};

/* Create a stack with given capacity */
struct alstack* alstack_create(int cap)
{
        if (cap <= 0)
                return NULL;

        /* Allocate memory for the stack structure */
        struct alstack* list = malloc(sizeof(struct alstack));
        if (!list)
                return NULL;

        /* Initialize members */
        list->size = 0;
        list->capacity = cap;

        /* Allocate memory for the points array */
        list->points = malloc(sizeof(struct point) * (cap + 1));
        if (!list->points) {
                free(list);
                return NULL;
        }

        return list;
}

/* Push a point onto the stack */
void alstack_push(struct alstack* list, struct point point)
{
        if (list->size == list->capacity) exit(1);

        list->points[++list->size] = point;
}

/* Pop a point from the stack */
struct point alstack_pop(struct alstack* list)
{
        if (list->size == 0) exit(1);

        return list->points[list->size--];
}

/* Peek at the top point of the stack */
struct point alstack_peek(struct alstack* list)
{
        if (list->size == 0) exit(1);

        return list->points[list->size];
}

/* Cut the stack to a new size */
void alstack_cut(struct alstack* list, int new_size)
{
        if (new_size < 0 || new_size > list->size) exit(1);

        list->size = new_size;
}

/* Free the stack */
void alstack_free(struct alstack* list)
{
        if (!list) return;

        free(list->points);
        list->points = NULL;
        free(list);
}

/* Get the next point in a given direction */
struct point next_point(struct point point, int direction)
{
        struct point next = point;
        if (direction == 1) {           // up
                next.row--;
        } else if (direction == 2) {    // down
                next.row++;
        } else if (direction == 3) {    // left          
                next.col--;
        } else if (direction == 4) {    // right
                next.col++;
        }
        return next;
}

/* Push unvisited neighbors of a point onto the to_visit stack */
int push_nbrs(int** maze, struct alstack* to_visit, struct point point)
{
        int n_added = 0;        // number of neighbors added

        /* check 4 directions: up, down, left, right */
        for (int direction = 1; direction <= 4; direction++) {
                struct point next = next_point(point, direction);
                if (maze_status(maze, next) == 0) {
                        maze_mark(maze, next);
                        next.prev_size = point.prev_size + 1;
                        alstack_push(to_visit, next);
                        n_added++;
                }
        }

        return n_added;
}

/* Find a path from start to end in the maze */
struct alstack* find(int** maze, struct point start, struct point end, int len)
{
        /* create the stacks */

        struct alstack* to_visit = alstack_create(len * len);
        if (!to_visit) return NULL;

        struct alstack* path = alstack_create(len * len);
        if (!path) {
                alstack_free(to_visit);
                return NULL;
        }

        /* initialize the search */
        start.prev_size = 0;
        maze_mark(maze, start);
        alstack_push(to_visit, start);

        /* search loop */
        while (to_visit->size > 0) {
                /* pop a point from the to_visit stack */
                struct point point = alstack_pop(to_visit);

                /* push the point onto the path stack */
                alstack_push(path, point);

                /* check if we reached the end */
                if (point.row == end.row && point.col == end.col) {
                        alstack_free(to_visit);
                        return path;
                }

                /* push unvisited neighbors onto the to_visit stack */
                int n = push_nbrs(maze, to_visit, point);

                /* if no neighbors were added, backtrack */
                if (n == 0) {
                        struct point top = alstack_peek(to_visit);
                        alstack_cut(path, top.prev_size);
                }
        }
        
        /* no path found */
        alstack_free(to_visit);
        alstack_cut(path, 0);
        return path;
}


void find_maze_stack(int** maze, int start_row, int start_col, int end_row, int end_col)
{
        /* implement stack-based maze solving */
        struct point start = { .row = start_row, .col = start_col };
        struct point end = { .row = end_row, .col = end_col };
        struct alstack* path = find(maze, start, end, 9);
        for(int i = 1; i <= path->size; i++) {
                struct point p = path->points[i];
                maze[p.row][p.col] = 2;   // mark the path in the maze
        }
        if (path) {
                alstack_free(path);
        }
}


void maze_mark2(int **maze, struct point point, int val)
{
        maze[point.row][point.col] = val;
}

struct point_queue {
        struct point *points;
        int front;
        int rear;
};

struct point_queue *point_queue_create(int cap)
{
        struct point_queue *q 
                = malloc(sizeof(struct point_queue));
        if (!q) return NULL;

        q->points = malloc(sizeof(struct point) * (cap + 1));
        if (!q->points) {
                free(q);
                return NULL;
        }

        q->front = 0;
        q->rear = 0;
        return q;
}

void point_queue_free(struct point_queue *q)
{
        if (!q) return;
        free(q->points);
        free(q);
}

void point_queue_enqueue(struct point_queue *q, struct point point)
{
        q->points[++q->rear] = point;
}

struct point point_queue_dequeue(struct point_queue *q)
{
        return q->points[++q->front];
}

struct point next_point2(struct point point, int direction)
{
        struct point next = point;
        next.prev_size++;
        if (direction == 1) {			// 상
                next.row--;
        } else if (direction == 2) {		// 하
                next.row++;
        } else if (direction == 3) {		// 좌
                next.col--;
        } else if (direction == 4) {		// 우
                next.col++;
        }
        return next;
}




void push_nbrs2(int** maze, int len, struct point_queue* to_visit, struct point point)
{
        int point_index = point.row * len + point.col;
        
        for (int direction = 1; direction <= 4; direction++) {
                struct point next = next_point2(point, direction);
                if (maze_status(maze, next) == 0) {
                        maze_mark2(maze, next,point_index);
                        point_queue_enqueue(to_visit, next);
                }
        }
}


int find2(int** maze, int len, struct point start, struct point end)
{
        struct point_queue *to_visit 
                = point_queue_create(len * len);
        if (!to_visit) return -2;		// 에러 코드

        start.prev_size = 0;
        maze_mark2(maze, start, -1);
        point_queue_enqueue(to_visit, start);

        while (to_visit->front < to_visit->rear) {
               struct point p = point_queue_dequeue(to_visit);
               if (p.row == end.row && p.col == end.col) {
                        point_queue_free(to_visit);
                        return p.prev_size;    // 경로 거리
               }
               push_nbrs2(maze, len, to_visit, p);
        }
        point_queue_free(to_visit);
        return -1;
}


struct point *decode_path(int **maze, int len, struct point end, int path_len) {
        struct point *path = malloc(sizeof(struct point) * (path_len + 1));
        if (!path) return NULL;
        struct point point = end;
        for (int i = path_len; i >= 0; i--) {
                path[i] = point;
                int prev_data = maze_status(maze, point);
                if (prev_data < 0) break;
                point.row = prev_data / len;
                point.col = prev_data % len;
        }
        return path;
}


void find_maze_queue(int** maze, int start_row, int start_col, int end_row, int end_col)
{
        /* implement queue-based maze solving */
        struct point start = { .row = start_row, .col = start_col };
        struct point end = { .row = end_row, .col = end_col };
        int path_len = find2(maze, 9, start, end);
        if (path_len >= 0) {
                struct point *path = decode_path(maze, 9, end, path_len);
                if (path) {
                        for (int i = 0; i <= path_len; i++) {
                                struct point p = path[i];
                                maze[p.row][p.col] = 2;   // mark the path in the maze
                        }
                        free(path);
                }
        }
}


/* Print the maze (len:9) to the terminal */
void print_maze_9(int** maze)
{
        int nrows = 9;
        int ncols = 9;

        /* print * for wall, blank for open space, o for path */
        for (int row = 0; row < nrows; row++) {
                for (int col = 0; col < ncols; col++) {
                        if (maze[row][col] == 1)
                                printf("*");
                        else if (maze[row][col] == 0)
                                printf(" ");
                        else if (maze[row][col] == 2)
                                printf("o");
                        else
                                printf(" ");
                }
                printf("\n");
        }
}

/* Test the maze solving algorithm */
void maze_test_9x9()
{
        /* define a simple maze (9x9) */
        int maze1[9][9] = {
                {1, 1, 1, 1, 1, 1, 1, 1, 1},
                {1, 0, 0, 0, 1, 0, 0, 0, 1},
                {1, 0, 1, 0, 1, 0, 1, 1, 1},
                {1, 0, 1, 0, 0, 0, 0, 1, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 0, 1, 1},
                {1, 1, 1, 1, 1, 1, 0, 1, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 1, 1, 1, 1, 1, 1, 1, 1}
        };

        /* convert maze to array of pointers for compatibility */
        int *maze_row_ptrs[9];
        for(int i = 0; i < 9; i++)
                maze_row_ptrs[i] = maze1[i];

        /* print the maze */
        printf("Maze:\n");
        print_maze_9(maze_row_ptrs);
        printf("\n\n");

        int start_row = 1, start_col = 1;
        int end_row = 7, end_col = 7;

        /* solve the maze using stack-based approach */
        printf("Solving maze using stack-based approach:\n");

        find_maze_stack(maze_row_ptrs, start_row, start_col, end_row, end_col);

        print_maze_9(maze_row_ptrs);
        printf("\n\n");

        /* reset the maze */
        int maze2[9][9] = {
                {1, 1, 1, 1, 1, 1, 1, 1, 1},
                {1, 0, 0, 0, 1, 0, 0, 0, 1},
                {1, 0, 1, 0, 1, 0, 1, 1, 1},
                {1, 0, 1, 0, 0, 0, 0, 1, 1},
                {1, 0, 1, 1, 1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 0, 1, 1},
                {1, 1, 1, 1, 1, 1, 0, 1, 1},
                {1, 0, 0, 0, 0, 0, 0, 0, 1},
                {1, 1, 1, 1, 1, 1, 1, 1, 1}
        };
        for(int i = 0; i < 9; i++)
                maze_row_ptrs[i] = maze2[i];

        /* solve the maze using queue-based approach */
        printf("Solving maze using queue-based approach:\n");

        find_maze_queue(maze_row_ptrs, start_row, start_col, end_row, end_col);

        print_maze_9(maze_row_ptrs);
        printf("\n\n");

        return;
}


/*       
        * - Main function 
*/

/* Hide cursor, test maze, and reset terminal */
int main(int argc, char** argv)
{
        maze_test_9x9();
        return 0;
}
