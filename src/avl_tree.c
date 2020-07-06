#include "avl_tree.h"

extern avl_tree_t global_avl_tree;

size_t avl_tree_insert(char* base, size_t size, size_t tag){
    char* end = base+size;
    global_avl_tree.root = node_insert(global_avl_tree.root, base, end, tag);
    return tag;
}

avl_node_t* node_insert(avl_node_t* root, char* base, char* end, size_t tag){
    if(root == NULL){
        root = &global_avl_tree.nodes[global_avl_tree.size++];
        root->base = base;
        root->end = end;
        root->left = root->right = NULL;
        root->tag = tag;
        root->height = 1;
        return root;
    }

    if(root->base > base){
        root->left = node_insert(root->left, base, end, tag);
    }else if(root->end <= base){
        root->right = node_insert(root->right, base, end, tag);
    }else{
        root->base = base;
        root->end = end;
        root->tag = tag;
        return root;
    }

    root->height = node_max(node_height(root->left), node_height(root->right))+1;

    int balance = node_balance(root);

    if(balance > 1 && base < root->left->base){
        return node_rotate_right(root);
    }

    if(balance < -1 && base >= root->right->end){
        return node_rotate_left(root);
    }

    if(balance > 1 && base >= root->left->end){
        root->left = node_rotate_left(root->left);
        return node_rotate_right(root);
    }

    if(balance < -1 && base < root->right->base){
        root->right = node_rotate_right(root->right);
        return node_rotate_left(root);
    }

    return root;
}

void avl_tree_remove(char* base){
    global_avl_tree.root = node_delete(global_avl_tree.root, base);
}

avl_node_t* node_delete(avl_node_t* root, char* base){
    if(root == NULL){
        return NULL;
    }

    if(root->base > base){
        root->left = node_delete(root->left, base);
    }else if(root->end <= base){
        root->right = node_delete(root->right, base);
    }else if(root->left && root->right){
        avl_node_t* temp = root;
        root = find_min(root->right);
        root->left = temp->left;
        root->right = node_delete(temp->right, root->base);
    }else{
        root = root->left? root->left: root->right;
    }

    if(root == NULL){
        return root;
    }

    root->height = max(node_height(root->left), node_height(root->right))+1;

    int balance = node_balance(root);

    if(balance > 1 && node_balance(root->left) >= 0){
        return node_rotate_right(root);
    }

    if(balance > 1 && node_balance(root->left) < 0){
        root->left = node_rotate_left(root->left);
        return node_rotate_right(root);
    }

    if(balance < -1 && node_balance(root->right) <= 0){
        return node_rotate_left(root);
    }

    if(balance < -1 && node_balance(root->right) > 0){
        root->right = node_rotate_right(root->right);
        return node_rotate_left(root);
    }

    return root;
}

size_t avl_tree_search(char* base){
    avl_node_t* node = node_search(global_avl_tree.root, base);
    if(node == NULL){
        return -1;
    }

    return node->tag;
}

avl_node_t* node_search(avl_node_t* root, char* base){
    if(root == NULL){
        return root;
    }

    if(root->base > base){
        return node_search(root->left, base);
    }else if(root->end <= base){
        return node_search(root->right, base);
    }

    return root;
}

