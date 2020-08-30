#ifndef BST_H
#define BST_H 1
#include "default.h"
typedef struct _tree_node
{
    int key;
    char data[ANTFX_MAX_STR_BUFF_SZ];
    struct _tree_node* left;
    struct _tree_node* right;
} bst_node_t;

void bst_free(bst_node_t* root);
bst_node_t* bst_insert(bst_node_t* root, int key, const char* data);
bst_node_t* bst_find_min(bst_node_t* root);
bst_node_t* bst_find_max(bst_node_t* root);
bst_node_t* bst_find(bst_node_t* root, int x);
bst_node_t* bst_delete(bst_node_t* root, int x);
void bst_for_each(bst_node_t* root, void (*callback)(bst_node_t*, void **, int), void** args, int argc);
#endif