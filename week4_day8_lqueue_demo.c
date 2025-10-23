#include <stdio.h>
#include <stdlib.h>

enum Foreground {
        FG_WHITE = 231,
        FG_BLACK = 239,
        FG_BLACK_REAL = 0,
        FG_RED = 1,
        FG_GREEN = 2, 
        FG_DARK_YELLOW = 3,
        FG_BLUE = 4, 
        FG_PURPLE = 5,
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
        printf("\x1B[%d;%dH", 1 + y, 1 + x);
}

void _hide_cursor()
{
        printf("\x1B[?25l");
}

void _show_cursor()
{
        printf("\x1B[?25h");
}

void erase_line()
{
        printf("\x1b[2K");
}

void init_screen()
{
        struct Color background = { .bg = BG_WHITE, .fg = FG_WHITE };
        set_color(&background);

        for(int i = 0; i <= 5; i++) {
                _cursor_pos(i, 0);
                erase_line();
        }
}

void erase_node(unsigned int cnt)
{
        struct Color background = { .bg = BG_WHITE, .fg = FG_WHITE };
        set_color(&background);

        _cursor_pos(2, 10 + cnt * 10);
        printf("          ");
        _cursor_pos(11, 0);
}

void erase_node_and_flush(unsigned int cnt)
{
        erase_node(cnt);
        _cursor_pos(11, 0);
        fflush(stdout);
}

void print_node(unsigned int cnt, int val, int next)
{
        erase_node(cnt);

        struct Color border_color = { .bg = BG_WHITE, .fg = FG_DARK_YELLOW };
        struct Color member_color = { .bg = BG_WHITE, .fg = FG_BLACK_REAL };

        _cursor_pos(2, 10 + cnt * 10);

        set_color(&border_color);
        printf("[");
        
        set_color(&member_color);
        printf("%d", val);
        
        set_color(&border_color);
        printf(", ");
        
        set_color(&member_color);
        if (next == 0)
                printf("NULL");
        else
                printf("&%d", next);
        
        set_color(&border_color);
        printf("]");

        _cursor_pos(11, 0);
        fflush(stdout);
}


void print_node_next_only(unsigned int cnt, int val, int next)
{
        struct Color border_color = { .bg = BG_WHITE, .fg = FG_DARK_YELLOW };
        struct Color member_color = { .bg = BG_WHITE, .fg = FG_BLACK_REAL };

        _cursor_pos(2, 10 + cnt * 10 + 4);
        
        set_color(&member_color);
        if (next == 0)
                printf("NULL");
        else
                printf("&%d", next);
        
        set_color(&border_color);
        printf("]  ");

        _cursor_pos(11, 0);
        fflush(stdout);
}

void print_q(unsigned int front, unsigned int rear)
{
        struct Color color = { .bg = BG_WHITE, .fg = FG_RED };
        set_color(&color);

        _cursor_pos(3, 10);
        erase_line();

        _cursor_pos(4, 10);
        erase_line();

        _cursor_pos(3, 12 + front * 10);
        printf("^");

        _cursor_pos(4, 8 + front * 10);
        printf("front");

        _cursor_pos(3, 8 + rear * 10 + 8);
        printf("^");

        _cursor_pos(4, 8 + rear * 10 + 8);
        printf("rear");

        _cursor_pos(11, 0);
        fflush(stdout);
}

void print_q_update_front(unsigned int front, unsigned int rear, unsigned int old_front)
{
        struct Color background = { .bg = BG_WHITE, .fg = FG_WHITE };
        set_color(&background);

        _cursor_pos(3, 12 + old_front * 10);
        printf(" ");

        _cursor_pos(4, 8 + old_front * 10);
        printf("     ");


        struct Color color = { .bg = BG_WHITE, .fg = FG_RED };
        set_color(&color);

        _cursor_pos(3, 12 + front * 10);
        printf("^");

        _cursor_pos(4, 8 + front * 10);
        printf("front");

        _cursor_pos(11, 0);
        fflush(stdout);
}

void print_q_update_rear(unsigned int front, unsigned int rear, unsigned int old_rear)
{
        struct Color background = { .bg = BG_WHITE, .fg = FG_WHITE };
        set_color(&background);

        _cursor_pos(3, 8 + old_rear * 10 + 8);
        printf(" ");

        _cursor_pos(4, 8 + old_rear * 10 + 8);
        printf("    ");

        struct Color color = { .bg = BG_WHITE, .fg = FG_RED };
        set_color(&color);

        _cursor_pos(3, 8 + rear * 10 + 8);
        printf("^");

        _cursor_pos(4, 8 + rear * 10 + 8);
        printf("rear");

        _cursor_pos(11, 0);
        fflush(stdout);
}

void erase_error_msg()
{
        wait(0);

        _cursor_pos(1, 1);
        erase_line();

        _cursor_pos(11, 0);
        fflush(stdout);
}

void print_error_msg(const char* str)
{
        struct Color color = { .bg = BG_WHITE, .fg = FG_BLUE };

        _cursor_pos(1, 1);
        erase_line();

        _cursor_pos(1, 1);
        set_color(&color);
        printf("%s", str);

        _cursor_pos(11, 0);
        fflush(stdout);
}

void print_return_value(int value)
{
        wait(0);

        struct Color color = { .bg = BG_WHITE, .fg = FG_BLUE };

        _cursor_pos(1, 1);
        erase_line();

        _cursor_pos(1, 1);
        set_color(&color);
        printf("RETURN: %d", value);

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



struct lqueue {
        struct node *front;
        struct node *rear;
        int size;
};

struct node {
        struct node *next;
        int value;
};

struct node *node_create(int value, struct node *next)
{
        struct node *node = malloc(sizeof(struct node));
        if (!node) exit(1);
        node->value = value;
        node->next = next;
        return node;
}

static int wait(int seed)
{
        ++seed;
        ++seed;
        for (int i = 1; i < 100000; i++)                 // change 10000 to a larger number to slow down
                for (int j = 1; j < 10000; j++)
                        seed *= i * j;
        return seed;
}

void node_print(struct node* node)
{
        int v = node->value;
        int n = 0;
        if (node->next)
                n = node->next->value;
        wait(0);
        print_node(v, v, n);
}

void node_print_update_next(struct node* node)
{
        int v = node->value;
        int n = 0;
        if (node->next)
                n = node->next->value;
        wait(0);
        print_node_next_only(v, v, n);
}

void node_erase(struct node* node)
{
        int v = node->value;
        int n = 0;
        if (node->next)
                n = node->next->value;
        wait(0);
        erase_node_and_flush(v);
}


struct lqueue *lqueue_create()
{
        struct lqueue *q = malloc(sizeof(struct lqueue));
        if (!q) exit(1);
        q->front = q->rear = node_create(0, NULL); // 더미
        q->size = 0;
        return q;
}

void lqueue_print(struct lqueue* q)
{
        wait(0);
        print_q(q->front->value, q->rear->value);
}

void lqueue_print_update_front(struct lqueue* q, struct node* old_node)
{
        wait(0);
        print_q_update_front(q->front->value, q->rear->value, old_node->value);
}

void lqueue_print_update_rear(struct lqueue* q, struct node* old_node)
{
        wait(0);
        print_q_update_rear(q->front->value, q->rear->value, old_node->value);
}


struct lqueue *lqueue_create_demo()
{
        struct lqueue *q = lqueue_create();
        node_print(q->front);
        lqueue_print(q);
        return q;
}

void lqueue_enqueue(struct lqueue *q, int value)
{
        if (!q) exit(1);
        q->rear->next = node_create(value, NULL);
        q->rear = q->rear->next;
        ++q->size;
}

void lqueue_enqueue_visual(struct lqueue *q, int value)
{
        if (!q) exit(1);
        q->rear->next = node_create(value, NULL);
        node_print(q->rear->next);
        node_print_update_next(q->rear);

        struct node *old_rear = q->rear;
        q->rear = q->rear->next;
        lqueue_print_update_rear(q, old_rear);

        ++q->size;
}

void lqueue_enqueue_demo(struct lqueue *q)
{
        for(int i = 1; i <= 3; i++)
                lqueue_enqueue_visual(q, i);
}

int lqueue_dequeue(struct lqueue *q)
{
        if (!q) exit(1);
        struct node *old_head = q->front;
        if (!old_head->next) exit(1);
        q->front = old_head->next;
        q->size--;
        free(old_head);
        return q->front->value;
}

int lqueue_dequeue_visual(struct lqueue *q)
{
        if (!q) exit(1);
        struct node *old_head = q->front;
        if (!old_head->next) print_error_msg("ERROR: EMPTY QUEUE");
        q->front = old_head->next;
        lqueue_print_update_front(q, old_head);

        q->size--;

        node_erase(old_head);
        free(old_head);

        print_return_value(q->front->value);
        erase_error_msg();
        return q->front->value;
}

void lqueue_dequeue_demo(struct lqueue *q)
{
        for(int i = 0; i < 3; i++) {
                lqueue_dequeue_visual(q);
        }
}


int main(int argc, char** argv)
{
        _hide_cursor();
        init_screen();
        struct lqueue *q = lqueue_create_demo();
        lqueue_enqueue_demo(q);
        lqueue_dequeue_demo(q);
        reset_terminal();
        return 0;
}

