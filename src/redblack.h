#ifndef TMTE_REDBLACK_H
#define TMTE_REDBLACK_H

#include <sys/types.h>
#include "config.h"
#include "chunk.h"
#include "state.h"

/*
 * [PROG]: Red Black Tree
 * [AUTHOR]: Ashfaqur Rahman <sajib.finix@gmail.com>
 * [PURPOSE]: Red-Black tree is an algorithm for creating a balanced
 *   binary search tree data structure. Implementing a red-balck tree
 *   data structure is the purpose of this program.
 * 
 * [DESCRIPTION]: Its almost like the normal binary search tree data structure. But
 *   for keeping the tree balanced an extra color field is introduced to each node.
 *  This tree will mantain bellow properties.
 *  1. Nodes can be either RED or BLACK.
 *  2. ROOT is BLACK.
 *  3. Leaves of this tree are null nodes. Here null is represented bya special node NILL.
 *     Each NILL nodes are BLACK. So each leave is BLACK.
 *   4. Each RED node's parent is BLACK
 *  5. Each simple path taken from a node to descendent leaf has same number of black height. 
 *     That means each path contains same number of BLACK nodes.
 */

#define RED 0
#define BLACK 1
//64bit architecture
#define UNMAP_UNIT_POWER ((size_t)12)
#define UNMAP_UNIT (size_t)((size_t)1<<UNMAP_UNIT_POWER)


#define ENCODE_BIT ((size_t)1)<<63
#define ENCODE_MASK (~((size_t)ENCODE_BIT))
#define COLOR_BIT (((size_t)1)<<62)
#define COLOR_MASK (~((size_t)COLOR_BIT))
//#define KEY_BITS (size_t)-1 >> UNMAP_UNIT_POWER
//#define KEY_MASK ~((size_t)KEY_BITS)
#define LR_MASK ((size_t)-1 << 48)
#define LR_BITS (~((size_t)LR_MASK))
#define EXH_BITS (LR_MASK)
#define EXH_MASK (LR_BITS)

#define GET_ENC(n)\
  (int)((*(size_t*)(n) & ENCODE_BIT) >> 63)

#define SET_ENC(n, c)\
  (*(size_t*)(n)) = (*(size_t*)(n) & ENCODE_MASK) | ((size_t)c<<63)

#define GET_COLOR(n)\
  (int)((*(size_t*)(n) & COLOR_BIT) >> 62)

#define SET_COLOR(n, c)\
  (*(size_t*)(n)) = (*(size_t*)(n) & COLOR_MASK) | ((size_t)c<<62)

#define GET_KEY(n)\
  ((size_t)n)>>UNMAP_UNIT_POWER

//K >> 12
//#define SET_KEY(n, k)                                         \
//  ((struct node*)n)->head = (((n)->head) & KEY_MASK | k)
#define GET_EXH(n)\
  (size_t) ( ( (size_t) ((n)->right) & EXH_BITS)>>48)
//  (size_t)(((n)->head & EXH_BITS) >> (64-UNMAP_UNIT_POWER))
// E >> 4

#define SET_EXH(n, v)\
  (n)->right = (struct node *) ( ((size_t)((n)->right) & EXH_MASK) | (((size_t)v)<<48) )
  //  (n)->head = (n)->head & EXH_MASK | (e<<(64-UNMAP_UNIT_POWER))

/*tag preservation*/
//#define GET_P(n)                                      \
//  (struct node *)((size_t)((n)->parent) & TAG_MASK)

//((size_t)((n)->parent)) | ((size_t)p)

//#define GET_L(n)                                      \
//(struct node *)((size_t)((n)->left) & TAG_MASK)

#define SET_L(n, l)\
  (n)->left = (((size_t)(n)->left) & LR_MASK )| ((size_t)l)
//((size_t)((n)->left)) | ((size_t)l)
//#define GET_R(n)                                      \
//  (struct node *)((size_t)((n)->right) & TAG_MASK)
#define SET_R(n, r)\
  (n)->right = (((size_t)(n)->right) & LR_MASK )|((size_t)r)
//((size_t)((n)->right)) | ((size_t)r)

#define GET_L(n)\
  (((size_t) ( (n)->left) ) & LR_BITS)
#define GET_R(n)\
  ( ((size_t)((n)->right)) & LR_BITS)

struct node{
  struct node *left;//encoding | color | left bits(48 bits)
  struct node *right;//size(8bit) | right bits(48 bits)
  struct node *parent;
};

struct small_node{//for 16byte limited space. broken encoding => need full node migration later
  struct node *left;//encoding | color | left
  struct node *right;//value | right
};


/*iyb: for debug*/
struct node* GET_P(struct node* n);
void SET_P(struct node* n, struct node* p);

/* Global, since all function will access them */
struct node *ROOT;
struct node *NILL;
struct node global_node;

static inline int init_redblack_tree(){
  SET_COLOR(&global_node, BLACK);
  //  struct node[NUM_TREE_NODES] TREE;
  NILL = &global_node;
  //struct node* NILL;

  ROOT = NILL;
  /*
  printf("### RED-BLACK TREE INSERT ###\n\n");

  int tcase, key;
  printf("Number of key: ");
  scanf("%lld", &tcase);
  struct node* array = (struct node*)malloc(sizeof(struct node)*tcase);
  while(tcase--){
    printf("Enter key: ");
    scanf("%lld", &key);
    red_black_insert(key, (struct node*)array++);
  }
  printf("### TREE PRINT ###\n\n");
  tree_print(ROOT);
  printf("\n");

  printf("### KEY SEARCH ###\n\n");
  printf("Enter key: ");
  scanf("%lld", &key);
  printf((tree_search(key) == NILL) ? "NILL\n" : "%p\n", tree_search(key));

  printf("### MIN TEST ###\n\n");
  printf("MIN: %lld\n", (tree_minimum(ROOT))->key);

  printf("### TREE DELETE TEST ###\n\n");
  printf("Enter key to delete: ");
  scanf("%lld", &key);
  red_black_delete(tree_search(key));

  printf("### TREE PRINT ###\n\n");
  tree_print(ROOT);
  printf("\n");
  */
  return 0;
}

                 void left_rotate(struct node *x);
                 void right_rotate(struct node *x);
                 void tree_print(struct node *x);
                 void red_black_insert(size_t key, size_t exh, struct node *z);
                 void red_black_insert_fixup(struct node *z);
                 struct node *tree_search(size_t key);
                 struct node *parent_search(size_t key);
                 struct node *tree_minimum(struct node *x);
                 void red_black_transplant(struct node *u, struct node *v);
                 void red_black_delete(struct node *z);
                 void red_black_delete_fixup(struct node *x);

                 size_t invalidate_chunk(struct malloc_state* m, struct malloc_chunk* chunk);

#if DBG
extern int num_mmap;
#endif
#endif
