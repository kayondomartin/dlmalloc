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
//64bit architecture
#define UNMAP_UNIT_POWER (size_t)12
#define UNMAP_UNIT (size_t)(2^UNMAP_UNIT_POWER)


#define COLOR_BIT 1<<63
#define COLOR_MASK ~((size_t)COLOR_BIT)
#define KEY_BITS (size_t)-1 >> UNMAP_UNIT_POWER
#define KEY_MASK ~((size_t)KEY_BITS)
#define EXH_BITS (size_t)-1 & COLOR_MASK & KEY_MASK
#define EXH_MASK ~((size_t)EXH_BITS)

#define GET_COLOR(p)\
  (int)((p)->head >> 63)
#define SET_COLOR(p, c)\
  ((struct node*)p)->head = (p)->head & COLOR_MASK | (size_t)c<<63
#define GET_KEY(p)\
  (size_t)((p)->head & KEY_BITS)
//K >> 12
#define SET_KEY(p, k)\
  ((struct node*)p)->head = (((p)->head) & KEY_MASK | k)
#define GET_EXH(p)\
  (size_t)(((p)->head & EXH_BITS) >> 64-UNMAP_UNIT_POWER)
// E >> 4
#define SET_EXH(p, e)\
  (p)->head = (p)->head & EXH_MASK | (e<<(64-UNMAP_UNIT_POWER))

struct node{
  size_t head;//topmost:color_bit / lower 48 key_bit / rest size>>4
  struct node *parent;
  struct node *left;
  struct node *right;
};

/* Global, since all function will access them */
struct node *ROOT;
struct node *NILL;
struct node global_node;

void left_rotate(struct node *x);
void right_rotate(struct node *x);
void tree_print(struct node *x);
void red_black_insert(size_t key, size_t exh, struct node *z);
void red_black_insert_fixup(struct node *z);
struct node *tree_search(size_t key);
struct node *tree_minimum(struct node *x);
void red_black_transplant(struct node *u, struct node *v);
void red_black_delete(struct node *z);
void red_black_delete_fixup(struct node *x);

size_t invalidate_chunk(struct malloc_state* m, struct malloc_chunk* chunk);
