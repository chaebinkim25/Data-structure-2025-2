#include <stdio.h>
#define WIDTH 32
#define HEIGHT 9

struct tree_node {
    int data;
    struct tree_node *left;
    struct tree_node *right;
    int height;
};

char texts[HEIGHT][WIDTH + 5];

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
    texts[y_node][x + 1] = '(';
    texts[y_node][x + 2] = '0' + node->height;
    texts[y_node][x + 3] = ')';

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
    reset_texts();

    int x = (WIDTH + 1) / 2;
    int y = 0;

    if (root) {
        texts[y][x] = '0' + root->data;
        texts[y][x + 1] = '(';
        texts[y][x + 2] = '0' + root->height;
        texts[y][x + 3] = ')';
    }
    else {
        texts[y][x] = '.';
    }

    if (root->left) {
        print_tree_visit(root->left, 1, 0);
    }
    if (root->right) {
        print_tree_visit(root->right, 1, 1);
    }

    print_texts();
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

struct tree_node *add_node_bst(struct tree_node *root, struct tree_node *node)
{
    if (!root)
        root = node;

    if (!node || node->data == root->data)
        ;

    else if (node->data < root->data)
        root->left = add_node_bst(root->left, node);
    
    else
        root->right = add_node_bst(root->right, node);

    return root;    
}


int get_height(struct tree_node *node)
{
    return node ? node->height : 0;
}

void set_height(struct tree_node *node)
{
    int left_height = get_height(node->left);
    int right_height = get_height(node->right);

    if (left_height > right_height)
        node->height = left_height + 1;
    else
        node->height = right_height + 1;
}

void set_height_recursive(struct tree_node *root)
{
    if (!root) return;

    set_height_recursive(root->left);
    set_height_recursive(root->right);

    set_height(root);
    printf("node %d(bf: %d)\n", root->data, balance_factor(root));
}

int balance_factor(struct tree_node *node)
{
    printf("balance factor of %d (%p, %p): %d\n", node->data, node->left, node->right, get_height(node->left) - get_height(node->right));
    return get_height(node->left) - get_height(node->right);
}

struct tree_node *add_node(struct tree_node *root, struct tree_node *node);

struct tree_node *_add_node(struct tree_node *root, struct tree_node *node)
{
    if (!node || node->data == root->data) {
        return root;
    }
    else if (node->data < root->data) {
        root->left = add_node(root->left, node);
    }
    else {
        root->right = add_node(root->right, node);
    }
    return root;
}

struct tree_node *ll_rotate(struct tree_node *node)
{
    struct tree_node *root = node->left;
    node->left = root->right;
    root->right = node;

    set_height(node);
    set_height(root);

    return root;
}

struct tree_node *lr_rotate(struct tree_node *node)
{
    struct tree_node *l = node->left;
    struct tree_node *root = l->right;

    l->right = root->left;
    root->left = l;
    
    node->left = root->right;
    root->right = node;

    set_height(l);
    set_height(node);
    set_height(root);

    return root;
}

struct tree_node *rr_rotate(struct tree_node *node)
{
    struct tree_node *root = node->right;
    node->right = root->left;
    root->left = node;

    set_height(node);
    set_height(root);

    return root;
}

struct tree_node *rl_rotate(struct tree_node *node)
{
    printf("RL rotation of %d\n", node->data);

    struct tree_node *r = node->right;
    struct tree_node *root = r->left;


    r->left = root->right;
    root->right = r;

    node->right = root->left;
    root->left = node;

    set_height(r);
    set_height(node);
    set_height(root);

    return root;
}

struct tree_node *rebalance(struct tree_node *node)
{
    int bf = balance_factor(node);

    if (bf < 2 && bf > -2) return node;

    print_tree(node);

    if (bf >= 2) {
        struct tree_node *l = node->left;

        if (get_height(l->left) >= get_height(l->right)) {
            printf("LL rotation\n");
            node = ll_rotate(node);
        }
        else {
            printf("LR rotation\n");
            node = lr_rotate(node);
        }
    }

    else if (bf <= -2) {
        struct tree_node *r = node->right;

        if (get_height(r->right) >= get_height(r->left)) {
            printf("RR rotation\n");
            node = rr_rotate(node);
        }
        else {
            printf("RL rotation\n");
            node = rl_rotate(node);
        }
    }

    return node;
}

struct tree_node *add_node(struct tree_node *root, struct tree_node *node)
{
    if (!root)
        root = node;
    else
        root = _add_node(root, node);

    set_height(root);

    return rebalance(root);
}

struct tree_node *leftmost_leaf(struct tree_node *node) {
    while (node->left) {
        node = node->left;
    }
    return node;
}

struct tree_node *delete_node(struct tree_node *root, struct tree_node *node);

struct tree_node *delete_root(struct tree_node *root)
{
    if (!root->left && !root->right) {
        root = NULL;
    }
    else if (!root->left && root->right) {
        root = root->right;
    }
    else if (root->left && !root->right) {
        root = root->left;
    }
    else {
        struct tree_node *next = leftmost_leaf(root->right);
        root->right = delete_node(root->right, next);

        next->left = root->left;
        next->right = root->right;

        root = next;
    }
    return root;
}

struct tree_node *delete_node(struct tree_node *root, struct tree_node *node)
{
    if (!root) return NULL;
    if (!node) return root;

    if (node->data < root->data) {
        root->left = delete_node(root->left, node);
    }
    else if (node->data > root->data) {
        root->right = delete_node(root->right, node);
    }
    else {
        root = delete_root(node);
    }

    if (!root)
        return root;

    set_height(root);
    return rebalance(root);
}


void print_node(struct tree_node *node)
{
    char data = '0' + node->data;
    char height = '0' + node->height;
    char left = node->left ? '0' + node->left->data : '.';
    char right = node->right ? '0' + node->right->data : '.';
    printf("node[%c] (%c, %c)\n", data, left, right);
}


void reset_nodes(struct tree_node *arr, int size)
{
    for (int i = 0; i < size; i++) {
        arr[i].data = i;
        arr[i].left = NULL;
        arr[i].right = NULL;
        arr[i].height = 0;
    }
}

int main() {
    return 0;
}