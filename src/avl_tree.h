#ifndef __AVL_TREE_H
#define __AVL_TREE_H

#include <unistd.h>
#include <sys/mman.h>
#define SOFTBOUNDCETS_MMAP_FLAGS (MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE)

typedef struct avl_node avl_node_t;

struct avl_node {
    char* base;
    char* end;
    size_t tag;
    size_t height;
    avl_node_t* left;
    avl_node_t* right;
};

typedef struct avl_tree avl_tree_t;

struct avl_tree {
    size_t size;
    avl_node_t* root;
    avl_node_t* nodes;
};

extern avl_tree_t global_avl_tree;
avl_tree_t global_avl_tree;

static inline void init_avl_tree(){
    global_avl_tree.size = 0;
    global_avl_tree.root = NULL;
    global_avl_tree.nodes = (avl_node_t*)mmap(0, 0x0000010000000000, PROT_READ | PROT_WRITE, SOFTBOUNDCETS_MMAP_FLAGS, -1, 0);
}

static inline int node_height(avl_node_t* node){
    if(node == NULL){
        return 0;
    }

    return node->height;
}

static inline avl_node_t* find_min(avl_node_t* node){
    if(node->left == NULL){
        return node;
    }

    return find_min(node->left);
}

static inline size_t node_max(size_t a, size_t b){
    return a > b? a: b;
}

static inline avl_node_t* node_rotate_right(avl_node_t* node){
    avl_node_t* left = node->left;
    avl_node_t* left_right = left->right;

    left->right = node;
    node->left = left_right;

    node->height = node_max(node_height(node->left), node_height(node->right))+1;
    left->height = node_max(node_height(left->left), node_height(left->right))+1;

    return left;
}

static inline avl_node_t* node_rotate_left(avl_node_t* node){
    avl_node_t* right = node->right;
    avl_node_t* right_left = right->left;

    right->left = node;
    node->right = right_left;

    node->height = node_max(node_height(node->left), node_height(node->right))+1;
    right->height = node_max(node_height(right->left), node_height(right->right))+1;

    return right;
}

static inline int node_balance(avl_node_t* node){
    if(node == NULL){
        return 0;
    }

    return node_height(node->left)-node_height(node->right);
}

size_t avl_tree_insert(char* base, size_t size, size_t tag);
void avl_tree_remove(char* base);
size_t avl_tree_search(char* base);
avl_node_t* node_insert(avl_node_t* root, char* base, char* end, size_t tag);
avl_node_t* node_delete(avl_node_t* root, char* base);
avl_node_t* node_search(avl_node_t* root, char* base);
#endif