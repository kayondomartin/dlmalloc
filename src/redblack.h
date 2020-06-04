#include <sys/types.h>
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
#define UNMAP_UNIT (size_t)4U*(size_t)1024U
#define NUM_TREE_NODES 200

struct node{
  size_t key;
  int color;
  size_t exhausted_size;
  struct node *parent;
  struct node *left;
  struct node *right;
};


/* Global, since all function will access them */
struct node *ROOT;
struct node *NILL;
extern struct node global_node;

void left_rotate(struct node *x);
void right_rotate(struct node *x);
void tree_print(struct node *x);
void red_black_insert(size_t key, struct node *z);
void red_black_insert_fixup(struct node *z);
struct node *tree_search(size_t key);
struct node *tree_minimum(struct node *x);
void red_black_transplant(struct node *u, struct node *v);
void red_black_delete(struct node *z);
void red_black_delete_fixup(struct node *x);

size_t invalidate_chunk(struct malloc_state* m, struct malloc_chunk* chunk);
