#include <stdio.h>
#include <stdlib.h>


/*
 * Index
 *
        * - Terminal settings
        * - Maze solving
        * - Main function
 */


/* 
        * - Terminal settings 
        *
                * -- Terminal color setting
                * -- Terminal cursor setting
                * -- Terminal printing utility
                * -- Terminal reset
 */


/*                -- Terminal color setting */

enum Foreground {
        FG_WHITE = 231,
        FG_BLACK = 239,
};

enum Background {
        BG_WHITE = 231,
        BG_BLACK = 239,
        BG_GREY = 248,
        BG_BLUE = 39,
        BG_GREEN = 120,
        BG_RED = 210,
};

struct Color {
        enum Foreground fg;
        enum Background bg;
};

void set_color(const struct Color* color)
{
        printf("\x1B[38;5;%dm", color->fg);     // set foreground color
        printf("\x1B[48;5;%dm", color->bg);     // set background color
}

void reset_color()
{
        printf("\x1B[0m");                      // reset all attributes
}


/*                -- Terminal cursor setting */

/* 
 * Set cursor position 
 * y: row (0-based)
 * x: column (0-based, each column is 2 characters wide)
 */
void _cursor_pos(unsigned int y, unsigned int x)
{
        printf("\x1B[%d;%dH", 1 + y, 1 + 2 * x);        // ANSI escape code is 1-based
}

void _hide_cursor()
{
        printf("\x1B[?25l");
}

void _show_cursor()
{
        printf("\x1B[?25h");
}


/*                -- Terminal printing utility */

/*
 * Print a string at a specific row and column with specified colors
        * y: row (0-based)
        * x: column (0-based, each column is 2 characters wide)
        * str: string to print (should be 2 characters wide)
        * fg: foreground color
        * bg: background color
 */
void print_rc_str(unsigned int y, unsigned int x, const char* str, enum Foreground fg, enum Background bg)
{
        struct Color color = { .bg = bg, .fg = fg };
        set_color(&color);
        _cursor_pos(y, x);
        printf("%2s", str);
        _cursor_pos(11, 0);     // Move cursor out of the way   
        fflush(stdout);
}

/*
 * Print a string at a specific row and column with specified colors
        * y: row (0-based)
        * x: column (0-based, each column is 2 characters wide)
        * i: integer to print (should be from -9 to 99)
        * fg: foreground color
        * bg: background color
 */
void print_rc_int(unsigned int y, unsigned int x, unsigned int i, enum Foreground fg, enum Background bg)
{
        struct Color color = { .bg = bg, .fg = fg };
        set_color(&color);
        _cursor_pos(y, x);
        printf("%2d", i);
        _cursor_pos(11, 0);     // Move cursor out of the way
        fflush(stdout);
}


/*                -- Terminal reset */

void reset_terminal()
{
        reset_color();
        _show_cursor();
        _cursor_pos(12, 0);   // Move cursor to the bottom
        fflush(stdout);        
}


/*
        * - Maze solving
                * -- maze (2d array) utilities
                * -- stack of points (array list implementation)
                * -- visualization of stack
                * -- maze solving algorithm
                * -- maze visualization and test
 */


/*                -- maze (2d array) utilities */

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


/*                -- visualization of stack */

/* A dummy wait function to slow down the visualization */
static int wait(int seed)
{
        ++seed;
        ++seed;
        for (int i = 1; i < 50000; i++)                 // change 10000 to a larger number to slow down
                for (int j = 1; j < 10000; j++)
                        seed *= i * j;
        return seed;
}

/* Push a point onto the to_visit stack and show it */
void alstack_push_to_visit_show(struct alstack* to_visit, struct point point)
{
        alstack_push(to_visit, point);

        struct Color color = { .bg = BG_GREEN, .fg = FG_BLACK };        // green for to_visit

        /* change the mark of old_top from ** to stack size */
        if (to_visit->size > 0) { 
                struct point old_top = alstack_peek(to_visit);
                print_rc_int(old_top.row, old_top.col, to_visit->size, color.fg, color.bg);
        }
        wait(0);

        /* show the new point as ** */
        print_rc_str(point.row, point.col, "**", color.fg, color.bg);

        wait(0);
}

/* Pop a point from the to_visit stack and show it */
struct point alstack_pop_to_visit_show(struct alstack* to_visit)
{
        struct point point = alstack_pop(to_visit);

        struct Color color = { .bg = BG_RED, .fg = FG_WHITE };       // red for popped from to_visit
        print_rc_str(point.row, point.col, "**", color.fg, color.bg);

        return point;
}

/* Push a point onto the path stack and show it */
void alstack_push_path_show(struct alstack* path, struct point point)
{
        alstack_push(path, point);

        struct Color color = { .bg = BG_BLUE, .fg = FG_WHITE };      // blue for path
        print_rc_int(point.row, point.col, path->size, color.fg, color.bg);

        wait(0);
}

/* Cut the path stack to a new size and show it */
void alstack_cut_path_show(struct alstack* path, int new_size)
{
        /* pop and show the cut points one by one */
        while (path->size > new_size) {
                struct point point = alstack_pop(path);

                struct Color color = { .bg = BG_GREY, .fg = FG_BLACK };     // grey for cut from path
                print_rc_str(point.row, point.col, "  ", color.fg, color.bg);
                wait(0);
        }

        wait(0);
}


/*                -- maze solving algorithm */

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
                        alstack_push_to_visit_show(to_visit, next);
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
        alstack_push_to_visit_show(to_visit, start);

        /* search loop */
        while (to_visit->size > 0) {
                /* pop a point from the to_visit stack */
                struct point point = alstack_pop_to_visit_show(to_visit);

                /* push the point onto the path stack */
                alstack_push_path_show(path, point);

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
                        alstack_cut_path_show(path, top.prev_size);
                }
        }
        
        /* no path found */
        alstack_free(to_visit);
        alstack_cut(path, 0);
        return path;
}


/*                -- maze visualization and test */

/* Print the maze to the terminal */
void print_maze(int** maze, int nrows, int ncols)
{
        struct Color color = { .bg = BG_WHITE, .fg = FG_BLACK };

        /* loop over maze */
        for (int row = 0; row < nrows; row++) {
                for (int col = 0; col < ncols; col++) {

                        /* set color based on maze cell value */
                        if (maze[row][col] == 1) {      // wall is black
                                color.bg = BG_BLACK;
                                color.fg = FG_WHITE;
                        } else {                        // path is white
                                color.bg = BG_WHITE;
                                color.fg = FG_BLACK;
                        }
                        set_color(&color);

                        /* print the cell */
                        _cursor_pos(row, col);
                        printf("  ");
                }
                printf("\n");
        }
}

/* Test the maze solving algorithm */
void maze_test()
{
        /* define a simple maze (7x7) */
        int maze[7][7] = {
                {1, 1, 1, 1, 1, 1, 1},
                {1, 0, 1, 0, 1, 1, 1},
                {1, 0, 1, 0, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 1},
                {1, 1, 1, 1, 1, 1, 1},
        };

        /* convert maze to array of pointers for compatibility */
        int *maze_row_ptrs[7];
        for(int i = 0; i < 7; i++)
                maze_row_ptrs[i] = maze[i];

        /* print the maze */
        print_maze(maze_row_ptrs, 7, 7);

        /* define start and end points */
        struct point start = { 1, 1, 0 };
        struct point end = { 5, 5, 0 };

        /* print start point */
        wait(0);
        print_rc_str(start.row, start.col, "ss", FG_BLACK, BG_WHITE);

        /* print end point */
        wait(0);
        print_rc_str(end.row, end.col, "dd", FG_BLACK, BG_WHITE);

        /* find the path */
        wait(0);
        struct alstack* path = find(maze_row_ptrs, start, end, 7);

        /* print the path */
        wait(0);
        wait(0);
        reset_color();

        if (!path) {
                printf("path alloc failed\n");
                return;
        }

        if (path->size == 0) {
                printf("no path\n");
        } else {
                for (int i = 1; i <= path->size; i++)
                        printf("(%d,%d), ", path->points[i].row,
                                path->points[i].col);
                printf("\n");
        }

        /* free the path stack */
        alstack_free(path);

        return;
}


/*       
        * - Main function 
*/

/* Hide cursor, test maze, and reset terminal */
int main(int argc, char** argv)
{
        _hide_cursor();
        maze_test();
        reset_terminal();
        return 0;
}
