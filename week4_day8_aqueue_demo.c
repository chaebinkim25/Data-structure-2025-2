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

void print_data(int *data, int len)
{
        struct Color border_color = { .bg = BG_WHITE, .fg = FG_DARK_YELLOW };
        struct Color member_color = { .bg = BG_WHITE, .fg = FG_BLACK_REAL };

        _cursor_pos(2, 10);

        set_color(&border_color);
        printf("[");
        
        set_color(&member_color);
        if (data[0] == 0)
                printf("  ");
        else
                printf("%2d", data[0]);
        
        for(int i = 1; i < len; i++) {

                set_color(&border_color);
                printf(",");

                set_color(&member_color);
                if (data[i] == 0)
                        printf("  ");
                else
                        printf("%2d", data[i]);

        }        
        
        set_color(&border_color);
        printf("]");

        _cursor_pos(11, 0);
        fflush(stdout);
}


void print_data_change(int *data, int len, int i)
{
        struct Color border_color = { .bg = BG_WHITE, .fg = FG_DARK_YELLOW };
        struct Color member_color = { .bg = BG_WHITE, .fg = FG_BLACK_REAL };

        _cursor_pos(2, 11 + i * 3);
        
        set_color(&member_color);
        if (data[i] == 0)
                printf("  ");
        else
                printf("%2d", data[i]);

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

        _cursor_pos(3, 11 + front * 3);
        printf("^");

        _cursor_pos(4, 7 + front * 3);
        printf("front");

        _cursor_pos(3, 12 + rear * 3);
        printf("^");

        _cursor_pos(4, 12 + rear * 3);
        printf("rear");

        _cursor_pos(11, 0);
        fflush(stdout);
}

void print_q_update_front(unsigned int front, unsigned int rear, unsigned int old_front)
{
        struct Color background = { .bg = BG_WHITE, .fg = FG_WHITE };
        set_color(&background);

        _cursor_pos(3, 11 + old_front * 3);
        printf(" ");

        _cursor_pos(4, 7 + old_front * 3);
        printf("     ");


        struct Color color = { .bg = BG_WHITE, .fg = FG_RED };
        set_color(&color);

        _cursor_pos(3, 11 + front * 3);
        printf("^");

        _cursor_pos(4, 7 + front * 3);
        printf("front");

        _cursor_pos(11, 0);
        fflush(stdout);
}

void print_q_update_rear(unsigned int front, unsigned int rear, unsigned int old_rear)
{
        struct Color background = { .bg = BG_WHITE, .fg = FG_WHITE };
        set_color(&background);

        _cursor_pos(3, 12 + old_rear * 3);
        printf(" ");

        _cursor_pos(4, 12 + old_rear * 3);
        printf("    ");

        struct Color color = { .bg = BG_WHITE, .fg = FG_RED };
        set_color(&color);

        _cursor_pos(3, 12 + rear * 3);
        printf("^");

        _cursor_pos(4, 12 + rear * 3);
        printf("rear");

        _cursor_pos(11, 0);
        fflush(stdout);
}

void erase_error_msg()
{
        wait(0);

        struct Color background = { .bg = BG_WHITE, .fg = FG_WHITE };
        set_color(&background);
        
        _cursor_pos(1, 1);
        erase_line();

        _cursor_pos(11, 0);
        fflush(stdout);
}

void print_error_msg(const char* str)
{
        wait(0);

        struct Color background = { .bg = BG_WHITE, .fg = FG_WHITE };
        set_color(&background);

        _cursor_pos(1, 1);
        erase_line();

        _cursor_pos(1, 1);

        struct Color color = { .bg = BG_WHITE, .fg = FG_BLUE };
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


struct aqueue {
        int *data;
        int capacity;
        int front;
        int rear;
        int size;
};


static int wait(int seed)
{
        ++seed;
        ++seed;
        for (int i = 1; i < 30000; i++)                 // change 10000 to a larger number to slow down
                for (int j = 1; j < 10000; j++)
                        seed *= i * j;
        return seed;
}


struct aqueue *aqueue_create(int cap)
{
        struct aqueue *q = malloc(sizeof(struct aqueue));
        if (!q) exit(1);

        q->data = calloc(cap + 1, sizeof(int)*(cap + 1));
        if (!q->data) exit(1);

        q->capacity = cap;

        q->front = 0;
        q->rear = 0;
        q->size = 0;

        return q;
}

void aqueue_print(struct aqueue* q)
{
        wait(0);
        print_data(q->data, q->capacity + 1);
        wait(0);
        print_q(q->front, q->rear);
}

void aqueue_print_update_front(struct aqueue* q, int old_front)
{
        wait(0);
        print_q_update_front(q->front, q->rear, old_front);
}

void aqueue_print_update_rear(struct aqueue* q, int old_rear)
{
        wait(0);
        print_q_update_rear(q->front, q->rear, old_rear);
}

void aqueue_print_change_data(struct aqueue* q, int i)
{
        wait(0);
        print_data_change(q->data, q->capacity, i);
}

struct aqueue *aqueue_create_demo()
{
        struct aqueue *q = aqueue_create(9);
        aqueue_print(q);
        return q;
}

void simple_aqueue_enqueue(struct aqueue *q, int v)
{
        if (!q || q->rear >= q->capacity) exit(1);

        q->data[++q->rear] = v;
        ++q->size;
}

void simple_aqueue_enqueue_visual(struct aqueue *q, int v)
{
        if (!q) exit(1);

        if (q->rear == q->capacity) {
                print_error_msg("ERROR: OVERFLOW");
                erase_error_msg();
                return;
        }

        int old_rear = q->rear;
        q->data[++q->rear] = v;
        aqueue_print_update_rear(q, old_rear);
        aqueue_print_change_data(q, q->rear);

        ++q->size;
}

void aqueue_enqueue_demo(struct aqueue *q)
{
        static int data = 1;

        for (int i = 1; i <= 5; i++)
                simple_aqueue_enqueue_visual(q, data++);
}

int simple_aqueue_dequeue(struct aqueue *q)
{
        if (!q) exit(1);
        if (q->front == q->rear) exit(1);

        int tmp = q->data[++q->front];
        q->size--;
        return tmp;
}

int simple_aqueue_dequeue_visual(struct aqueue *q)
{
        if (!q) exit(1);
        if (q->front == q->rear) {
                print_error_msg("ERROR: UNDERFLOW");
                erase_error_msg();
                return -1;
        }

        int old_front = q->front;
        int tmp = q->data[++q->front];
        aqueue_print_update_front(q, old_front);

        print_return_value(tmp);
        erase_error_msg();

        q->size--;
        return tmp;
}

void aqueue_dequeue_demo(struct aqueue *q)
{
        for (int i = 1; i <= 5; i++)
                simple_aqueue_dequeue_visual(q);
}

int main(int argc, char** argv)
{
        _hide_cursor();
        init_screen();
        struct aqueue *q = aqueue_create_demo();
        aqueue_enqueue_demo(q);
        aqueue_enqueue_demo(q);
        aqueue_dequeue_demo(q);
        aqueue_dequeue_demo(q);
        reset_terminal();
        return 0;
}

