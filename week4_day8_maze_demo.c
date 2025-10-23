#include <stdio.h>
#include <stdlib.h>


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
        printf("\x1B[38;5;%dm", color->fg);     
        printf("\x1B[48;5;%dm", color->bg);     
}

void reset_color()
{
        printf("\x1B[0m");                      
}




void _cursor_pos(unsigned int y, unsigned int x)
{
        printf("\x1B[%d;%dH", 1 + y, 1 + 2 * x);        
}

void _hide_cursor()
{
        printf("\x1B[?25l");
}

void _show_cursor()
{
        printf("\x1B[?25h");
}


void print_rc_str(unsigned int y, unsigned int x, const char* str, enum Foreground fg, enum Background bg)
{
        struct Color color = { .bg = bg, .fg = fg };
        set_color(&color);
        _cursor_pos(y, x);
        printf("%2s", str);
        _cursor_pos(11, 0);     
        fflush(stdout);
}

void print_rc_int(unsigned int y, unsigned int x, unsigned int i, enum Foreground fg, enum Background bg)
{
        struct Color color = { .bg = bg, .fg = fg };
        set_color(&color);
        _cursor_pos(y, x);
        printf("%2d", i);
        _cursor_pos(11, 0);     
        fflush(stdout);
}




void reset_terminal()
{
        reset_color();
        _show_cursor();
        _cursor_pos(12, 0);   
        fflush(stdout);        
}





struct point {
        int row;
        int col;
        int prev_size;  
};


int maze_status(int** maze, struct point point)
{
        return maze[point.row][point.col];
}


void maze_mark(int **maze, struct point point, int val)
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

static int wait(int seed)
{
        ++seed;
        ++seed;
        for (int i = 1; i < 50000; i++)                 
                for (int j = 1; j < 10000; j++)
                        seed *= i * j;
        return seed;
}

void point_queue_enqueue_visualization(struct point_queue *q, struct point point)
{
        q->points[++q->rear] = point;

        wait(0);
        struct Color color = { .bg = BG_GREEN, .fg = FG_BLACK };        // green for to_visit
        print_rc_int(point.row, point.col, point.prev_size, color.fg, color.bg);
}

struct point point_queue_dequeue_visualization(struct point_queue *q)
{
        struct point point = q->points[++q->front];

        wait(0);
        struct Color color = { .bg = BG_RED, .fg = FG_BLACK };        // green for to_visit
        print_rc_int(point.row, point.col, point.prev_size, color.fg, color.bg);

        return point;
}


struct point next_point(struct point point, int direction)
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


void push_nbrs(int** maze, int len, struct point_queue* to_visit, struct point point)
{
        int point_index = point.row * len + point.col;
        
        for (int direction = 1; direction <= 4; direction++) {
                struct point next = next_point(point, direction);
                if (maze_status(maze, next) == 0) {
                        maze_mark(maze, next,point_index);
                        point_queue_enqueue_visualization(to_visit, next);
                }
        }
}


int find(int** maze, int len, struct point start, struct point end)
{
        struct point_queue *to_visit 
                = point_queue_create(len * len);
        if (!to_visit) return -2;		// 에러 코드

        start.prev_size = 0;
        maze_mark(maze, start, -1);
        point_queue_enqueue_visualization(to_visit, start);

        while (to_visit->front < to_visit->rear) {
               struct point p = point_queue_dequeue_visualization(to_visit);
               if (p.row == end.row && p.col == end.col) {
                        point_queue_free(to_visit);
                        return p.prev_size;    // 경로 거리
               }
               push_nbrs(maze, len, to_visit, p);
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
                wait(0);
                struct Color color = { .bg = BG_BLUE, .fg = FG_BLACK };        // green for to_visit
                print_rc_str(point.row, point.col, "  ", color.fg, color.bg);

                int prev_data = maze_status(maze, point);
                if (prev_data < 0) break;
                point.row = prev_data / len;
                point.col = prev_data % len;
        }
        return path;
}



void print_maze(int** maze, int nrows, int ncols)
{
        struct Color color = { .bg = BG_WHITE, .fg = FG_BLACK };

        
        for (int row = 0; row < nrows; row++) {
                for (int col = 0; col < ncols; col++) {

                        
                        if (maze[row][col] == 1) {      
                                color.bg = BG_BLACK;
                                color.fg = FG_WHITE;
                        } else {                        
                                color.bg = BG_WHITE;
                                color.fg = FG_BLACK;
                        }
                        set_color(&color);

                        
                        _cursor_pos(row, col);
                        printf("  ");
                }
                printf("\n");
        }
}


void maze_test()
{
        
        int maze[7][7] = {
                {1, 1, 1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0, 0, 1},
                {1, 0, 1, 0, 1, 0, 1},
                {1, 0, 0, 0, 1, 0, 1},
                {1, 0, 1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1, 0, 1},
                {1, 1, 1, 1, 1, 1, 1},
        };

        int *m_row_ptrs[7];
        for(int i = 0; i < 7; i++)
                m_row_ptrs[i] = maze[i];

        print_maze(m_row_ptrs, 7, 7);

        
        struct point start = { 1, 1, 0 };
        struct point end = { 5, 5, 0 };

        
        wait(0);
        print_rc_str(start.row, start.col, "ss", FG_BLACK, BG_WHITE);

        
        wait(0);
        print_rc_str(end.row, end.col, "dd", FG_BLACK, BG_WHITE);

        
        wait(0);
        int path_len = find(m_row_ptrs, 7, start, end);

        
        wait(0);
        wait(0);
        reset_color();

        if (path_len < 0) {
               if (path_len == -1) printf("no path found\n");
               if (path_len == -2) printf("malloc failed\n");
               return;
        }

        printf("path_len = %d", path_len);

        struct point *path
                = decode_path(m_row_ptrs, 7, end, path_len);

        for (int i = 1; i <= path_len; i++)
                printf("(%d,%d), ", path[i].row, path[i].col);
        printf("\n");

       //리소스 정리
       free(path);
}


int main(int argc, char** argv)
{
        _hide_cursor();
        maze_test();
        reset_terminal();
        return 0;
}
