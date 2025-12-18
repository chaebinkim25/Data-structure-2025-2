#include <stdio.h>
#include <math.h>

struct hlist_node {
    struct hlist_node *next, **pprev;
    int key;
};

struct hlist_head {
    struct hlist_node *first;
};

void hlist_add_head (struct hlist_node *n, struct hlist_head *h)
{
    struct hlist_node *first = h->first;
    n->next = first;
    if (first)
        first->pprev = &n->next;
    
    h->first = n;
    n->pprev = &h->first;
}

#define GOLDEN_RATIO_32 0x61C88647

unsigned int hash_32(unsigned int val, unsigned int bits)
{
    val * GOLDEN_RATIO_32 >> (32 - bits);
}


void __hlist_del(struct hlist_node *n)
{
    struct hlist_node *next = n->next;
    struct hlist_node **pprev = n->pprev;

    *pprev = next;

    if (next)
        next->pprev = pprev;
}




int hlist_unhashed(struct hlist_node *h)
{
    return !h->pprev;
}

void init_hlist_node(struct hlist_node *h)
{
    h->next = NULL;
    h->pprev = NULL;
}

void hlist_del_init(struct hlist_node *n)
{
    if (!hlist_unhashed(n)) {
        __hlist_del(n);
        init_hlist_node(n);
    }
}


int hash_hashed(struct hlist_node *node)
{
    return !hlist_unhashed(node);
}

void hash_del(struct hlist_node *node)
{
    hlist_del_init(node);
}

int ilog2(unsigned int v)
{
    int l = 0;
    while ((1U << l) < v)
        l++;
    return l;
}
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define HASH_SIZE(name) (ARRAY_SIZE(name))
#define HASH_BITS(name) ilog2(HASH_SIZE(name))

void hash_add(struct hlist_head *hashtable, struct hlist_node *node, int key)
{
    hlist_add_head(node, &hashtable[hash_32(key, HASH_BITS(hashtable))]);
}

struct fox {
    char data;
    struct hlist_node n;
};

int main()
{
    printf("Hello, hash!\n");
    struct hlist_node n0 = {.next = NULL, .pprev = NULL, .key = 12345678};
    printf("%d\n", n0.key); 

    struct hlist_head head = {.first = &n0};
    n0.pprev = &head.first;
    printf("%d\n", head.first->key); 

    struct hlist_head hashtable[32];
    for (int i = 0; i < 32; i++) {
        hashtable[i].first = NULL;
    }

    hlist_add_head(&n0, &hashtable[hash_32(n0.key, 5)]);



    double phi = (3 - sqrt(5)) / 2;
    
    printf("%x\n", (unsigned int) ((1ull << 63) * phi));
    
    struct fox x = {.data = 'A'};
    struct hlist_node *n = &x.n;

    int offset = ((char *)(&x.n) - (char *) (&x));

    struct fox *pos = (struct fox*) ((char *)n - offset);

    printf("%c\n", pos->data);

    return 0;
}