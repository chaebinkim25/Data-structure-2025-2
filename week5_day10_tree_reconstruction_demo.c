#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

struct interval {
        int start;
        int end;
        int size;
};

enum Foreground {
        FG_WHITE = 231,
        FG_BLACK = 239,
        FG_RED = 210,
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

        for(int i = 0; i <= 15; i++) {
                _cursor_pos(i, 0);
                erase_line();
        }

        _cursor_pos(1, 0);

        struct Color color = { .bg = BG_WHITE, .fg = FG_BLACK };
        set_color(&color);
}

void print_rc_str(unsigned int y, unsigned int x, const char* str, enum Foreground fg, enum Background bg)
{
        struct Color color = { .bg = bg, .fg = fg };
        set_color(&color);
        _cursor_pos(y, x);
        printf("%s", str);
        fflush(stdout);
}

void print_rc_char(unsigned int y, unsigned int x, char c, enum Foreground fg, enum Background bg)
{
        struct Color color = { .bg = bg, .fg = fg };
        set_color(&color);
        _cursor_pos(y, x);
        printf("%c", c);
        fflush(stdout);
}

void print_preorder_chars(const char* chars)
{
        print_rc_str(1, 0, "preorder:", FG_BLACK, BG_WHITE);
        print_rc_str(1, 10, chars, FG_BLACK, BG_WHITE);
}

void print_preorder_char_done(char c, int index)
{
        print_rc_char(1, 10 + index, c, FG_RED, BG_WHITE);
}

void mark_left_preorder_chars(const char* chars, struct interval r)
{
        struct Color color = { .bg = BG_GREEN, .fg = FG_BLACK };
        set_color(&color);
        _cursor_pos(1, 10 + r.start);
        for (int i = r.start; i <= r.end; i++) {
                printf("%c", chars[i]);
        }
}

void mark_right_preorder_chars(const char* chars, struct interval r)
{
        struct Color color = { .bg = BG_BLUE, .fg = FG_BLACK };
        set_color(&color);
        _cursor_pos(1, 10 + r.start);
        for (int i = r.start; i <= r.end; i++) {
                printf("%c", chars[i]);
        }
}

void unmark_preorder_chars(const char* chars, struct interval r)
{
        struct Color color = { .bg = BG_GREY, .fg = FG_BLACK };
        set_color(&color);
        _cursor_pos(1, 10 + r.start);
        for (int i = r.start; i <= r.end; i++) {
                printf("%c", chars[i]);
        }
}

void print_inorder_chars(const char* chars)
{
        print_rc_str(3, 0, "inorder:", FG_BLACK, BG_WHITE);
        print_rc_str(3, 10, chars, FG_BLACK, BG_WHITE);
}

void print_inorder_char_done(char c, int index)
{
        print_rc_char(3, 10 + index, c, FG_RED, BG_WHITE);
}

void mark_left_inorder_chars(const char* chars, struct interval r)
{
        struct Color color = { .bg = BG_GREEN, .fg = FG_BLACK };
        set_color(&color);
        _cursor_pos(3, 10 + r.start);
        for (int i = r.start; i <= r.end; i++) {
                printf("%c", chars[i]);
        }
        _cursor_pos(11, 0);
}

void mark_right_inorder_chars(const char* chars, struct interval r)
{
        struct Color color = { .bg = BG_BLUE, .fg = FG_BLACK };
        set_color(&color);
        _cursor_pos(3, 10 + r.start);
        for (int i = r.start; i <= r.end; i++) {
                printf("%c", chars[i]);
        }
        _cursor_pos(11, 0);
}

void unmark_inorder_chars(const char* chars, struct interval r)
{
        struct Color color = { .bg = BG_GREY, .fg = FG_BLACK };
        set_color(&color);
        _cursor_pos(3, 10 + r.start);
        for (int i = r.start; i <= r.end; i++) {
                printf("%c", chars[i]);
        }
        _cursor_pos(11, 0);
}

void reset_terminal()
{
        reset_color();
        _show_cursor();
        _cursor_pos(20, 0);   
        fflush(stdout);        
}


struct tree_level_graphic_info {
        int level;
        int level_max_height;
        int level_row;
        int level_col_start;
        int level_col_space;
};       

struct tree_graphic_info {
        int tree_height;
        int max_level;
        int total_rows;
        int total_cols;
        struct tree_level_graphic_info* level_infos;
};

struct tree_graphic_info *compute_tree_graphic_info(int max_level)
{
        struct tree_graphic_info* info = malloc(sizeof(struct tree_graphic_info));
        info->max_level = max_level;
        info->tree_height = max_level - 1;
        info->total_rows = max_level * 2 - 1;
        info->total_cols = (1 << max_level) - 1;

        info->level_infos = malloc(sizeof(struct tree_level_graphic_info) * (max_level + 1));

        for (int level = 1; level <= max_level; level++) {
                struct tree_level_graphic_info* level_info = &info->level_infos[level];
                level_info->level = level;
                level_info->level_max_height = max_level - level;
                level_info->level_row = level * 2 - 1;
                level_info->level_col_start = (1 << level_info->level_max_height) - 1;
                level_info->level_col_space = 1 << (level_info->level_max_height + 1);
        }

        return info;
}

void show_tree_node(char c, int i, struct tree_graphic_info *tree_height)
{
        int level = 1;
        int index_end = 2;
        while (i >= index_end) {
                level++;
                index_end <<= 1;
        }

        struct tree_level_graphic_info *level_info = &tree_height->level_infos[level];

        int pos_in_level = i - (index_end / 2);

        int row = level_info->level_row;
        int col = level_info->level_col_start + pos_in_level * level_info->level_col_space;

        int row_offset = 0;
        int col_offset = 25;
        print_rc_char(row_offset + row, col_offset + col, c, FG_BLACK, BG_WHITE);

        int parent_index = i / 2;
        int is_right = i % 2;

        if (parent_index > 0) {
                struct tree_level_graphic_info *parent_level_info = &tree_height->level_infos[level - 1];
                int parent_pos_in_level = parent_index - (index_end / 4);
                int parent_row = parent_level_info->level_row;
                int parent_col = parent_level_info->level_col_start + parent_pos_in_level * parent_level_info->level_col_space;

                int link_row = parent_row + 1;
                int link_col;
                if (is_right) {
                        link_col = (col + parent_col + 1) / 2;
                        print_rc_char(link_row + row_offset, link_col + col_offset, '\\', FG_BLACK, BG_WHITE);
                } else {
                        link_col = (col + parent_col) / 2;
                        print_rc_char(link_row + row_offset, link_col + col_offset, '/', FG_BLACK, BG_WHITE);
                }
        }

}



inline struct interval range(int start, int end)
{
        struct interval r = { .start = start, .end = end, .size = end - start + 1 };
        return r;
}

int find_index(const char* arr, struct interval r, char val)
{
        for (int i = r.start; i <= r.end; i++) {
                if (arr[i] == val) {
                        return i;
                }
        }
        return -1;
}

struct interval find_range(const char* data, struct interval data_range, 
                            const char* targets, struct interval targets_range)
{
        int is_in_target[256] = { 0 };
        for (int i = targets_range.start; i <= targets_range.end; i++) {
                is_in_target[(unsigned char)targets[i]] = 1;
        }

        int i = data_range.start;
        while (i <= data_range.end) {
                if (is_in_target[(unsigned char)data[i]]) {
                        break;
                }
                i++;
        }

        return range(i, i + targets_range.size - 1);
}

void rebuild_tree(const char* preorder, struct interval pre_range, 
                  const char* inorder, struct interval in_range, 
                  char* tree, int root_index, struct tree_graphic_info *info)
{
        if (pre_range.size <= 0 || in_range.size <= 0) {
                return;
        }

        char root_val = preorder[pre_range.start];
        tree[root_index] = root_val;

        struct Color color = { .bg = BG_WHITE, .fg = FG_BLACK };
        set_color(&color);

        _cursor_pos(1, pre_range.start + 10);
        color.fg = FG_RED;
        color.bg = BG_WHITE;
        set_color(&color);
        printf("%c", root_val);
        fflush(stdout);
        Sleep(1000);

        show_tree_node(root_val, root_index, info);
        Sleep(1000);

        int root_inorder_index = find_index(inorder, in_range, root_val);
        _cursor_pos(3, root_inorder_index + 10);
        color.fg = FG_RED;
        color.bg = BG_WHITE;
        set_color(&color);
        printf("%c", root_val);
        fflush(stdout);
        Sleep(1000);

        if (root_inorder_index == -1) exit(1);

        struct interval left_in_range = range(in_range.start, root_inorder_index - 1);

        _cursor_pos(3, 10 + left_in_range.start);
        color.fg = FG_BLACK;
        color.bg = BG_GREEN;
        set_color(&color);
        for (int i = left_in_range.start; i <= left_in_range.end; i++) {
                printf("%c", inorder[i]);
        }
        fflush(stdout);
        Sleep(1000);

        struct interval left_pre_range = find_range(preorder, pre_range, inorder, left_in_range);

        _cursor_pos(1, 10 + left_pre_range.start);
        color.fg = FG_BLACK;
        color.bg = BG_GREEN;
        set_color(&color);
        for (int i = left_pre_range.start; i <= left_pre_range.end; i++) {
                printf("%c", preorder[i]);
        }
        fflush(stdout);
        Sleep(1000);

        rebuild_tree(preorder, left_pre_range, inorder, left_in_range, tree, root_index * 2, info);

        struct interval right_in_range = range(root_inorder_index + 1, in_range.end);
        _cursor_pos(3, 10 + right_in_range.start);
        color.fg = FG_BLACK;
        color.bg = BG_BLUE;
        set_color(&color);
        for (int i = right_in_range.start; i <= right_in_range.end; i++) {
                printf("%c", inorder[i]);
        }
        fflush(stdout);
        Sleep(1000);

        struct interval right_pre_range = find_range(preorder, pre_range, inorder, right_in_range);

        _cursor_pos(1, 10 + right_pre_range.start);
        color.fg = FG_BLACK;
        color.bg = BG_BLUE;
        set_color(&color);
        for (int i = right_pre_range.start; i <= right_pre_range.end; i++) {
                printf("%c", preorder[i]);
        }
        fflush(stdout);
        Sleep(1000);

        rebuild_tree(preorder, right_pre_range, inorder, right_in_range, tree, root_index * 2 + 1, info);
}

struct frame {
        struct interval pre_range;
        struct interval in_range;
        int root_index;
        int distance_from_tree_root;
};

void rebuild_tree_stack(const char* preorder, struct interval pre_range, 
                        const char* inorder, struct interval in_range, 
                        char* tree, struct tree_graphic_info *info)
{

        printf("Preorder: %s, Inorder: %s\n\n", preorder, inorder);

        struct frame stack[128];
        int stack_top = -1;

        ++stack_top;
        stack[stack_top].in_range = in_range;
        stack[stack_top].pre_range = pre_range;
        stack[stack_top].root_index = 1;

        while (stack_top >= 0) {
                struct frame *frame = &stack[stack_top];

                char root_val = preorder[frame->pre_range.start];
                tree[frame->root_index] = root_val;

                int root_inorder_index = find_index(inorder, frame->in_range, root_val);
                if (root_inorder_index == -1) { printf("Error: root value not found in inorder range\n"); exit(1);}

                struct interval left_in_range = range(frame->in_range.start, root_inorder_index - 1);
                printf("Left inorder range: [%d, %d], ", left_in_range.start, left_in_range.end);

                struct interval left_pre_range = find_range(preorder, frame->pre_range, inorder, left_in_range);
                printf("Left preorder range: [%d, %d]; ", left_pre_range.start, left_pre_range.end);

                struct interval right_in_range = range(root_inorder_index + 1, frame->in_range.end);
                printf("Right inorder range: [%d, %d], ", right_in_range.start, right_in_range.end);

                struct interval right_pre_range = find_range(preorder, frame->pre_range, inorder, right_in_range);
                printf("Right preorder range: [%d, %d]\n", right_pre_range.start, right_pre_range.end);

                int root_index = frame->root_index;
                printf("Placing %c at index %d\n", root_val, root_index);

                stack_top--;

                if (right_in_range.size > 0) {
                        stack_top++;
                        stack[stack_top].in_range = right_in_range;
                        stack[stack_top].pre_range = right_pre_range;
                        stack[stack_top].root_index = root_index * 2 + 1;
                }

                if (left_in_range.size > 0) {
                        stack_top++;
                        stack[stack_top].in_range = left_in_range;
                        stack[stack_top].pre_range = left_pre_range;
                        stack[stack_top].root_index = root_index * 2;
                }

        }

        printf("\n");
        for (int i = 1; i < 128; i++) {
                if (tree[i] != 0) {
                        // show_tree_node(tree[i], i, info);
                }
        }

        for (int i = 0; i < 20; i++) {
                printf("%2d ", i);
        }

        printf("\n");

        for (int i = 0; i < 20; i++) {
                printf("%2c ", tree[i] == 0 ? ' ' : tree[i]);
        }

        printf("\n\n");


}


void rebuild_tree_stack_visual(const char* preorder, struct interval pre_range, 
                               const char* inorder, struct interval in_range, 
                               char* tree, struct tree_graphic_info *info)
{
        struct Color color = { .bg = BG_WHITE, .fg = FG_BLACK };
        set_color(&color);

        _cursor_pos(1, 0);
        printf("Preorder:");

        _cursor_pos(7, 0);
        printf("Inorder:");

        struct frame stack[128];
        int stack_top = -1;

        ++stack_top;
        stack[stack_top].in_range = in_range;
        stack[stack_top].pre_range = pre_range;
        stack[stack_top].root_index = 1;
        stack[stack_top].distance_from_tree_root = 0;

        while (stack_top >= 0) {
                Sleep(2000);

                struct frame *frame = &stack[stack_top];

                color.fg = FG_BLACK;
                color.bg = BG_WHITE;
                set_color(&color);

                _cursor_pos(1 + frame->distance_from_tree_root, 10 + frame->pre_range.start);
                for(int i = frame->pre_range.start; i <= frame->pre_range.end; i++) {
                        printf("%c", preorder[i]);
                }
                _cursor_pos(7 + frame->distance_from_tree_root, 10 + frame->in_range.start);
                for(int i = frame->in_range.start; i <= frame->in_range.end; i++) {
                        printf("%c", inorder[i]);
                }
                fflush(stdout);
                Sleep(100);

                char root_val = preorder[frame->pre_range.start];

                color.fg = FG_RED;
                set_color(&color);
                _cursor_pos(1 + frame->distance_from_tree_root, 10 + frame->pre_range.start);
                printf("%c", root_val);
                _cursor_pos(2 + frame->distance_from_tree_root, 10 + frame->pre_range.start);
                printf("^");
                fflush(stdout);
                Sleep(100);

                tree[frame->root_index] = root_val;
                show_tree_node(root_val, frame->root_index, info);
                fflush(stdout);
                Sleep(100);

                int root_inorder_index = find_index(inorder, frame->in_range, root_val);
                if (root_inorder_index == -1) { printf("Error: root value not found in inorder range\n"); exit(1);}
                color.fg = FG_RED;
                set_color(&color);
                _cursor_pos(7 + frame->distance_from_tree_root, 10 + root_inorder_index);
                printf("%c", root_val);
                _cursor_pos(8 + frame->distance_from_tree_root, 10 + root_inorder_index);
                printf("^", root_val);
                fflush(stdout);
                Sleep(100);


                color.fg = FG_BLACK;
                color.bg = BG_GREEN;
                set_color(&color);

                struct interval left_in_range = range(frame->in_range.start, root_inorder_index - 1);
                _cursor_pos(7 + frame->distance_from_tree_root, 10 + left_in_range.start);
                for (int i = left_in_range.start; i <= left_in_range.end; i++) {
                        printf("%c", inorder[i]);
                }
                fflush(stdout);
                Sleep(100);

                struct interval left_pre_range = find_range(preorder, frame->pre_range, inorder, left_in_range);
                _cursor_pos(1 + frame->distance_from_tree_root, 10 + left_pre_range.start);
                for (int i = left_pre_range.start; i <= left_pre_range.end; i++) {
                        printf("%c", preorder[i]);
                }

                color.fg = FG_BLACK;
                color.bg = BG_BLUE;
                set_color(&color);

                struct interval right_in_range = range(root_inorder_index + 1, frame->in_range.end);
                _cursor_pos(7 + frame->distance_from_tree_root, 10 + right_in_range.start);
                for (int i = right_in_range.start; i <= right_in_range.end; i++) {
                        printf("%c", inorder[i]);
                }
                fflush(stdout);
                Sleep(100);

                struct interval right_pre_range = find_range(preorder, frame->pre_range, inorder, right_in_range);
                _cursor_pos(1 + frame->distance_from_tree_root, 10 + right_pre_range.start);
                for (int i = right_pre_range.start; i <= right_pre_range.end; i++) {
                        printf("%c", preorder[i]);
                }
                fflush(stdout);
                Sleep(100);

                int root_index = frame->root_index;
                int distance_from_tree_root = frame->distance_from_tree_root;
                stack_top--;

                if (right_in_range.size > 0) {
                        stack_top++;
                        stack[stack_top].in_range = right_in_range;
                        stack[stack_top].pre_range = right_pre_range;
                        stack[stack_top].root_index = root_index * 2 + 1;
                        stack[stack_top].distance_from_tree_root = distance_from_tree_root + 1;
                }

                if (left_in_range.size > 0) {
                        stack_top++;
                        stack[stack_top].in_range = left_in_range;
                        stack[stack_top].pre_range = left_pre_range;
                        stack[stack_top].root_index = root_index * 2;
                        stack[stack_top].distance_from_tree_root = distance_from_tree_root + 1;
                }

                printf("\n");
        }
}

void binary_tree_reconstruction_test()
{
        const char* inorder = "HDIBJEKALFMCNGO";
        const char* preorder = "ABDHIEJKCFLMGNO";


        struct tree_graphic_info* info = compute_tree_graphic_info(4);

        char* tree = calloc(1, sizeof(char) * 128);
        rebuild_tree_stack_visual(preorder, range(0, 14), inorder, range(0, 14), tree, info);

        printf("\n");

        free(tree);
        free(info->level_infos);
        free(info);
}

void test_ansi_bg()
{
        for (int bg = 0; bg <= 255; bg++) {
                struct Color color = { .bg = bg, .fg = FG_BLACK };
                set_color(&color);
                printf(" %3d ", bg);
                reset_color();
                printf(" ");
                if ((bg + 1) % 16 == 0) {
                        printf("\n");
                }
        }
        printf("\n");
}

int main(int argc, char** argv)
{
        _hide_cursor();
        init_screen();
        binary_tree_reconstruction_test();
        reset_terminal();
        return 0;
}
