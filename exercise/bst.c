#include <stdio.h>
#define WIDTH 32
#define HEIGHT 9

struct tree_node {
    int data;
    struct tree_node *left;
    struct tree_node *right;
    struct tree_node *parent;
    struct tree_node *prev;
    struct tree_node *next;
    struct tree_node **pparent;
};

char texts[HEIGHT][WIDTH + 1];

void reset_texts()
{
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            texts[i][j] = ' ';
        }
        texts[i][WIDTH] = '\0';
    }
}

void print_texts()
{
    for (int i = 0; i < HEIGHT; i++)
        printf("%s\n", texts[i]);
}

/* level starts from 0 */
/* level_order starts from 0 */
void print_tree_visit(struct tree_node *node, int level, int level_index)
{
    if (level >= (HEIGHT + 1) / 2)
        return;

    int level_capacity = 1 << level;
    int level_margin = WIDTH / level_capacity / 2;
    int x = level_margin + WIDTH * level_index / level_capacity;
    int y_edge = 2 * level - 1;
    int y_node = 2 * level;

    texts[y_node][x] = '0' + node->data;

    if (level_index % 2 == 0) {
        x += level_margin / 2;
        texts[y_edge][x] = '/';
    } else {
        x -= level_margin / 2;
        texts[y_edge][x] = '\\';
    }

    if (node->left)
        print_tree_visit(node->left, level + 1, level_index * 2);
    
    if (node->right)
        print_tree_visit(node->right, level + 1, level_index * 2 + 1);
}

void print_tree(struct tree_node *root)
{
    int x = (WIDTH + 1) / 2;
    int y = 0;

    if (root)
        texts[y][x] = '0' + root->data;
    else
        texts[y][x] = '.';

    if (root->left) {
        print_tree_visit(root->left, 1, 0);
    }
    if (root->right) {
        print_tree_visit(root->right, 1, 1);
    }

    print_texts();
}

void erase_tree_visit(struct tree_node *node, int level, int level_index)
{
    if (level >= (HEIGHT + 1) / 2)
        return;

    int level_capacity = 1 << level;
    int level_margin = WIDTH / level_capacity / 2;
    int x = level_margin + WIDTH * level_index / level_capacity;
    int y_edge = 2 * level - 1;
    int y_node = 2 * level;

    texts[y_node][x] = ' ';

    if (level_index % 2 == 0) {
        x += level_margin / 2;
        texts[y_edge][x] = ' ';
    } else {
        x -= level_margin / 2;
        texts[y_edge][x] = ' ';
    }

    if (node->left)
        erase_tree_visit(node->left, level + 1, level_index * 2);
    
    if (node->right)
        erase_tree_visit(node->right, level + 1, level_index * 2 + 1);
}



void erase_tree(struct tree_node *root)
{
    int x = WIDTH / 2;
    int y = 0;

    texts[y][x] = ' ';

    if (root) {
        if (root->left) {
            erase_tree_visit(root->left, 1, 0);
        }
        if (root->right) {
            erase_tree_visit(root->right, 1, 1);
        }
    }

    print_texts();
}


struct interval {
    int l;
    int r;
};

void set_childs_mid(struct tree_node *node_array, struct interval *range)
{
    int mid = (range->l + range->r + 1) / 2;

    struct interval left_range = {range->l, mid - 1};
    struct interval right_range = {mid + 1, range->r};
    
    printf("range: [%d, %d] (%d)\n", range->l, range->r, mid);

    if (left_range.l <= left_range.r) {
        int left_mid = (left_range.l + left_range.r + 1) / 2;
        node_array[mid].left = &node_array[left_mid];
        set_childs_mid(node_array, &left_range);
    }

    if (right_range.l <= right_range.r) {
        int right_mid = (right_range.l + right_range.r + 1) / 2;
        node_array[mid].right = &node_array[right_mid];
        set_childs_mid(node_array, &right_range);
    }
}


struct tree_node *balanced_bst_from_sorted_nums(struct tree_node *node_array, int *nums, int size)
{
    struct tree_node *nodes = node_array;

    for (int i = 0; i < size; i++) {
        nodes[i].data = nums[i];
        nodes[i].left = NULL;
        nodes[i].right = NULL;
    }


    struct interval range = {0, size - 1};
    set_childs_mid(node_array, &range);

    int mid = (range.l + range.r + 1) / 2;
    return &node_array[mid];
}

void _set_parent(struct tree_node *node)
{
    if (!node) return;

    if (node->left) {
        node->left->parent = node;
        _set_parent(node->left);
    }

    if (node->right) {
        node->right->parent = node;
        _set_parent(node->right);
    }
}

void set_parent(struct tree_node *root)
{
    if (!root) return;

    root->parent = NULL;

    _set_parent(root);
}


void _set_pparent(struct tree_node *node)
{
    if (!node) return;

    if (node->left) {
        node->left->pparent = &(node->left);
        _set_pparent(node->left);
    }

    if (node->right) {
        node->right->pparent = &(node->right);
        _set_pparent(node->right);
    }
}

void set_pparent(struct tree_node **root)
{
    if (!root) return;

    if (!*root) return;

    (*root)->pparent = root;

    _set_pparent(*root);
}

void _set_prev(struct tree_node *curr, struct tree_node **prev)
{
    if (!curr) return;

    _set_prev(curr->left, prev);

    curr->prev = *prev;
    *prev = curr;

    _set_prev(curr->right, prev);
}

void set_prev(struct tree_node *root)
{
    if (!root) return;

    struct tree_node *tmp = NULL;
    _set_prev(root, &tmp);
}

void _set_next(struct tree_node *curr, struct tree_node **prev)
{
    if (!curr) return;

    _set_next(curr->left, prev);

    if (*prev)
        (*prev)->next = curr;
    
    *prev = curr;

    _set_next(curr->right, prev);
}

void set_next(struct tree_node *root)
{
    if (!root) return;

    struct tree_node *tmp = NULL;
    _set_next(root, &tmp);
}

struct tree_node *search(struct tree_node *root, int key)
{
    if (!root) return NULL;

    if (key == root->data)
        return root;

    else if (key < root->data)
        return search(root->left, key);
        
    else
        return search(root->right, key);

    return NULL;
}

int add_node(struct tree_node *root, struct tree_node *node)
{
    if (!root) return -1;

    if (node->data == root->data) {
        return -2;
    }
    else if (node->data < root->data) {
        if (!root->left) {
            root->left = node;
            return 1;
        }
        else {
            return add_node(root->left, node);
        }
    }
    else {
        if (!root->right) {
            root->right = node;
            return 1;
        }
        else {
            return add_node(root->right, node);
        }
    }
}

void _delete_node(struct tree_node *node)
{
    if (node == NULL || node->pparent == NULL) {
        printf("delete_node : node is NULL or not connected\n");
        return;
    }

    if (node->left != NULL && node->right != NULL) {
        struct tree_node *replacer = node->next;

        if (replacer->left != NULL) {
            printf("delete_node : node has 2 childs. next has left child\n");
            return;
        }

        if (replacer != node->right) {
            *(replacer->pparent) = replacer->right;
            replacer->right = node->right;
        }

        replacer->left = node->left;
        *(node->pparent) = replacer;
    }
    else {
        struct tree_node *replacer = NULL;
        if (node->left)
            replacer = node->left;
        if (node->right)
            replacer = node->right;

        *(node->pparent) = replacer;
    }
}

void delete_node(struct tree_node **proot, struct tree_node *node)
{
    set_pparent(proot);
    set_next(*proot);

    _delete_node(node);
    node->pparent = NULL;
    node->parent = node->left = node->right = NULL;

    set_pparent(proot);
    set_parent(*proot);
    set_prev(*proot);
    set_next(*proot);
}

void print_node(struct tree_node *node)
{
    char data = '0' + node->data;
    char left = node->left ? '0' + node->left->data : '.';
    char right = node->right ? '0' + node->right->data : '.';
    char p = node->parent ? '0' + node->parent->data : '.';
    char con = node->pparent ? 'C' : 'D';
    char prev = node->prev ? '0' + node->prev->data : '.';
    char next = node->next ? '0' + node->next->data : '.';
    printf("node[%c] (%c, %c) \\ %c (%c) \\ %c, %c\n", data, left, right, p, con, prev, next);
}

int main() {
    reset_texts();
    texts[0][0] = 'H';
    texts[1][1] = 'E';
    texts[2][2] = 'L';
    texts[3][3] = 'L';
    texts[4][4] = 'O';
    texts[5][5] = '!';
    print_texts();

    reset_texts();

    struct tree_node nodes[10];
    for (int i = 0; i < 10; i++) {
        nodes[i].data = i;
        nodes[i].left = NULL;
        nodes[i].right = NULL;
        nodes[i].pparent = NULL;
        nodes[i].prev = NULL;
        nodes[i].next = NULL;
    }

    print_tree(&nodes[0]);
    print_tree(&nodes[1]);
    print_tree(&nodes[2]);

    for (int i = 0; i < 9; i++) {
        nodes[i].right = &nodes[i + 1];
    }

    print_tree(&nodes[0]);
    erase_tree(&nodes[0]);

    for (int i = 0; i < 10; i++) {
        nodes[i].data = -1;
        nodes[i].left = NULL;
        nodes[i].right = NULL;
    }

    int nums[10];
    for (int i = 0; i < 10; i++) {
        nums[i] = i;
    }

    struct tree_node *root = balanced_bst_from_sorted_nums(nodes, nums, 10);

    print_tree(root);

    set_parent(root);
    set_pparent(&root);

    struct tree_node *tmp = NULL;
    set_prev(root);
    tmp = NULL;
    set_next(root);

    for (int i = 0; i < 10; i++) {
        print_node(&nodes[i]);
    }

    struct tree_node *seven = search(root, 7);
    printf("search node 7 ... ");
    print_node(seven);

    struct tree_node a = {.data = 'y' - '0'};
    int add_result = add_node(root, &a);

    print_tree(root);

    delete_node(&root, &a);
    reset_texts();
    print_tree(root);

    delete_node(&root, &nodes[1]);
    reset_texts();
    print_tree(root);

    delete_node(&root, &nodes[2]);
    reset_texts();
    print_tree(root);

    delete_node(&root, &nodes[5]);
    reset_texts();
    print_tree(root);

    return 0;

}