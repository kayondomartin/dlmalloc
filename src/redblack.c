#include "redblack.h"
#include "os.h"
/*iyb: for debug*/
struct node* GET_P(struct node* n){//need to add inline at final step
  return (struct node *)((size_t)((n)->parent));
}
struct node* GET_L(struct node* n){
  return (struct node *)((size_t)((n)->left));
}
struct node* GET_R(struct node* n){
  return (struct node *)((size_t)((n)->right));
}
#if DBG
static size_t count = 0;
#endif
size_t invalidate_chunk(struct malloc_state* m, struct malloc_chunk* chunk){
  size_t ret = 0;
  size_t size = chunk_size(chunk);
  struct node* target = chunk;
  for(size_t i = (size_t)chunk >> UNMAP_UNIT_POWER;
      i <= ((size_t)chunk_plus_offset(chunk,size) -1) >> UNMAP_UNIT_POWER;
      i+=1, target = (struct node*)(i<<UNMAP_UNIT_POWER)){
    size_t start = (i>((size_t)chunk>>UNMAP_UNIT_POWER) ? i*UNMAP_UNIT : (size_t)chunk);
    size_t end = ((size_t)chunk + size > (i+1) * UNMAP_UNIT ? (i+1) * UNMAP_UNIT : (size_t)chunk + size);
    struct node * node_t = tree_search(i);
#if DBG
    count++;
    if(count>=18){
      dl_printf("");
    }
#endif
    if(node_t==NILL){
      red_black_insert(i, (end-start)>>4, (struct node*) target);
    }else{
      size_t size_h = GET_EXH(node_t);
      size_t size_n = (((end-start) >> 4) + size_h);
      if(size_n >= (UNMAP_UNIT>>4)){
        red_black_delete(node_t);
#if DBG
        dl_printf("iyb: unmapped %d times.\n", ++num_mmap);
#endif
        if(call_munmap(i*UNMAP_UNIT, UNMAP_UNIT) < 0){
          return -1;
        }
      }else{
        SET_EXH(node_t, size_n);
      }
    }
  }
  //TOP_FOOT_SIZE;
  return ret;
}

/* Print tree keys by inorder tree walk */
void tree_print(struct node *x){
  if(x != NILL){
    tree_print(GET_L(x));
    printf("%lu %lu\t", GET_KEY(x), GET_EXH(x));
    tree_print(GET_R(x));
  }
}

struct node *tree_search(size_t key){
  struct node *x;

  x = ROOT;
  while(x != NILL && GET_KEY(x) != key){
    if(key < GET_KEY(x)){
      x = GET_L(x);
    }
    else{
      x = GET_R(x);
    }
  }

  return x;
}

struct node *tree_minimum(struct node *x){
  while(GET_L(x) != NILL){
    SET_L(x, x);
  }
  return x;
}

/*
 * Insertion is done by the same procedure for BST Insert. Except new node is colored
 * RED. As it is coloured RED it may violate property 2 or 4. For this reason an
 * auxilary procedure called red_black_insert_fixup is called to fix these violation.
 */

void red_black_insert(size_t key, size_t exh, struct node*z){
  //  struct node *z, *x, *y;
  struct node *x, *y;
  //  z = malloc(sizeof(struct node));

  SET_EXH(z, exh);
  //  SET_KEY(z, key);
  SET_COLOR(z, RED);
  SET_L(z, NILL);
  SET_R(z, NILL);

  x = ROOT;
  y = NILL;

  /* 
   * Go through the tree untill a leaf(NILL) is reached. y is used for keeping
   * track of the last non-NILL node which will be z's parent.
   */
  while(x != NILL){
    y = x;
    if(GET_KEY(z) <= GET_KEY(x)){
      x = GET_L(x);
    }
    else{
      x = GET_R(x);
    }
  }

  if(y == NILL){
    ROOT = z;
  }
  else if(GET_KEY(z) <= GET_KEY(y)){
    SET_L(y, z);
  }
  else{
    SET_R(y, z);
  }

  SET_P(z, y);

  red_black_insert_fixup(z);
}

/*
 * Here is the psudocode for fixing violations.
 * 
 * while (z's parent is RED)
 *if(z's parent is z's grand parent's left child) then
 *if(z's right uncle or grand parent's right child is RED) then
 *make z's parent and uncle BLACK
 *make z's grand parent RED
 *make z's grand parent new z as it may violate property 2 & 4
 *(so while loop will contineue)
 *
 *else(z's right uncle is not RED)
 *if(z is z's parents right child) then
 *make z's parent z
 *left rotate z
 *make z's parent's color BLACK
 *make z's grand parent's color RED
 *right rotate z's grand parent
 *( while loop won't pass next iteration as no violation)
 *
 *else(z's parent is z's grand parent's right child)
 *do exact same thing above just swap left with right and vice-varsa
 *
 * At this point only property 2 can be violated so make root BLACK
 */

void red_black_insert_fixup(struct node *z){
  while(GET_COLOR(GET_P(z)) == RED){

    /* z's parent is left child of z's grand parent*/
    if(GET_P(z) == GET_L(GET_P(GET_P(z)))){

      /* z's grand parent's right child is RED */
      if(GET_COLOR( GET_R(GET_P(GET_P(z)))) == RED){
        SET_COLOR(GET_P(z), BLACK);
        SET_COLOR(GET_R(GET_P(GET_P(z))), BLACK);
        SET_COLOR(GET_P(GET_P(z)), RED);
        z = GET_P(GET_P(z));
      }

      /* z's grand parent's right child is not RED */
      else{

        /* z is z's parent's right child */
        if(z == GET_R(GET_P(z))){
          z = GET_P(z);
          left_rotate(z);
        }

        SET_COLOR(GET_P(z), BLACK);
        SET_COLOR(GET_P(GET_P(z)), RED);
        right_rotate(GET_P(GET_P(z)));
      }
    }

    /* z's parent is z's grand parent's right child */
    else{

      /* z's left uncle or z's grand parent's left child is also RED */
      if(GET_COLOR(GET_L(GET_P(GET_P(z)))) == RED){//bug
        SET_COLOR(GET_P(z), BLACK);
        SET_COLOR(GET_L(GET_P(GET_P(z))), BLACK);
        SET_COLOR(GET_P(GET_P(z)), RED);
        z = GET_P(GET_P(z));
      }

      /* z's left uncle is not RED */
      else{
        /* z is z's parents left child */
        if(z == GET_L(GET_P(z))){
          z = GET_P(z);
          right_rotate(z);
        }

        SET_COLOR(GET_P(z), BLACK);
        SET_COLOR(GET_P(GET_P(z)), RED);
        left_rotate(GET_P(GET_P(z)));
      }
    }
  }

  SET_COLOR(ROOT, BLACK);
}

/*
 * Lets say y is x's right child. Left rotate x by making y, x's parent and x, y's
 * left child. y's left child becomes x's right child.
 * 
 * xy
 *   / \                                 / \
 *  STA   y----------->  x   STC
 * /                                            \ / \
 *  STB   STC  STA   STB
 */

void left_rotate(struct node *x){
  struct node *y;

  /* Make y's left child x's right child */
  y = GET_R(x);
  SET_R(x, GET_L(y));
  if(GET_L(y) != NILL){
    SET_P(GET_L(y), x);
  }

  /* Make x's parent y's parent and y, x's parent's child */
  SET_P(y, GET_P(x));
  if(GET_P(y) == NILL){
    ROOT = y;
  }
  else if(x == GET_L(GET_P(x))){
    SET_L(GET_P(x), y);
  }
  else{
    SET_R(GET_P(x), y);
  }

  /* Make x, y's left child & y, x's parent */
  SET_L(y, x);
  SET_P(x, y);
}

/*
 * Lets say y is x's left child. Right rotate x by making x, y's right child and y
 * x's parent. y's right child becomes x's left child.
 *
 *||
 *xy
 *   /                                          \   / \
 *  y   STA---------------->STB  x
 * /                                            \ / \
 *  STB   STC  STC   STA
 */

void right_rotate(struct node *x){
  struct node *y;

  /* Make y's right child x's left child */
  y = GET_L(x);
  SET_L(x, GET_R(y));
  if(GET_R(y) != NILL){
    SET_P(GET_R(y), x);
  }

  /* Make x's parent y's parent and y, x's parent's child */
  SET_P(y, GET_P(x));
  if(GET_P(y) == NILL){
    ROOT = y;
  }
  else if(x == GET_L(GET_P(x))){
    SET_L(GET_P(x), y);
  }
  else{
    SET_R(GET_P(x), y);
  }
  
  /* Make y, x's parent and x, y's child */
  SET_R(y, x);
  SET_P(x, y);
}

/*
 * Deletion is done by the same mechanism as BST deletion. If z has no child, z is
 * removed. If z has single child, z is replaced by its child. Else z is replaced by
 * its successor. If successor is not z's own child, successor is replaced by its
 * own child first. then z is replaced by the successor.
 *
 * A pointer y is used to keep track. In first two case y is z. 3rd case y is z's
 * successor. So in first two case y is removed. In 3rd case y is moved.
 *
 *Another pointer x is used to keep track of the node which replace y.
 * 
 * As removing or moving y can harm red-black tree properties a variable
 * yOriginalColor is used to keep track of the original colour. If its BLACK then
 * removing or moving y harm red-black tree properties. In that case an auxilary
 * procedure red_black_delete_fixup(x) is called to recover this.
 */

void red_black_delete(struct node *z){
  struct node *y, *x;
  int yOriginalColor;

  y = z;
  yOriginalColor = GET_COLOR(y);

  if(GET_L(z) == NILL){
    x = GET_R(z);
    red_black_transplant(z, GET_R(z));
  }
  else if(GET_R(z) == NILL){
    x = GET_L(z);
    red_black_transplant(z, GET_L(z));
  }
  else{
    y = tree_minimum(GET_R(z));
    yOriginalColor = GET_COLOR(y);

    x = GET_R(y);

    if(GET_P(y) == z){
      SET_P(x, y);
    }
    else{
      red_black_transplant(y, GET_R(y));
      SET_R(y, GET_R(z));
      SET_P(GET_R(y), y);
    }

    red_black_transplant(z, y);
    SET_L(y, GET_L(z));
    SET_P(GET_L(y), y);
    SET_COLOR(y, GET_COLOR(z));
  }

  if(yOriginalColor == BLACK){
    red_black_delete_fixup(x);
  }
}

/*
 * As y was black and removed x gains y's extra blackness.
 * Move the extra blackness of x until
 *1. x becomes root. In that case just remove extra blackness
 *2. x becomes a RED and BLACK node. in that case just make x BLACK
 *
 * First check if x is x's parents left or right child. Say x is left child
 *
 * There are 4 cases.
 *
 * Case 1: x's sibling w is red. transform case 1 into case 2 by recoloring
 * w and x's parent. Then left rotate x's parent.
 *
 * Case 2: x's sibling w is black, w's both children is black. Move x and w's
 * blackness to x's parent by coloring w to RED and x's parent to BLACK.
 * Make x's parent new x.Notice if case 2 come through case 1 x's parent becomes 
 * RED and BLACK as it became RED in case 1. So loop will stop in next iteration.
 *
 * Case 3: w is black, w's left child is red and right child is black. Transform
 * case 3 into case 4 by recoloring w and w's left child, then right rotate w.
 *
 * Case 4: w is black, w's right child is red. recolor w with x's parent's color.
 * make x's parent BLACK, w's right child black. Now left rotate x's parent. Make x
 * point to root. So loop will be stopped in next iteration.
 *
 * If x is right child of it's parent do exact same thing swapping left<->right
 */

void red_black_delete_fixup(struct node *x){
  struct node *w;

  while(x != ROOT && GET_COLOR(x) == BLACK){

    if(x == GET_L(GET_P(x))){
      w = GET_R(GET_P(x));

      if(GET_COLOR(w) == RED){
        SET_COLOR(w, BLACK);
        SET_COLOR(GET_P(x), RED);
        left_rotate(GET_P(x));
        w = GET_R(GET_P(x));
      }

      if(GET_COLOR(GET_L(w)) == BLACK && GET_COLOR(GET_R(w)) == BLACK){
        SET_COLOR(w, RED);
        SET_COLOR(GET_P(x), BLACK);
        x = GET_P(x);
      }
      else{

        if(GET_COLOR(GET_R(w)) == BLACK){
          SET_COLOR(w, RED);
          SET_COLOR(GET_L(w), BLACK);
          right_rotate(w);
          w = GET_R(GET_P(x));
        }

        SET_COLOR(w, GET_COLOR(GET_P(x)));
        SET_COLOR(GET_P(x), BLACK);
        SET_COLOR(GET_R(x), BLACK);
        left_rotate(GET_P(x));
        x = ROOT;

      }

    }
    else{
      w = GET_L(GET_P(x));

      if(GET_COLOR(w) == RED){
        SET_COLOR(w, BLACK);
        SET_COLOR(GET_P(x), BLACK);
        right_rotate(GET_P(x));
        w = GET_L(GET_P(x));
      }
      
      if(GET_COLOR(GET_L(w)) == BLACK && GET_COLOR(GET_R(w)) == BLACK){
        SET_COLOR(w, RED);
        SET_COLOR(GET_P(x), BLACK);
        x = GET_P(x);
      }
      else{
        
        if(GET_COLOR(GET_L(w)) == BLACK){
          SET_COLOR(w, RED);
          SET_COLOR(GET_R(w), BLACK);
          left_rotate(w);
          w = GET_L(GET_P(x));
        }
        
        SET_COLOR(w, GET_COLOR(GET_P(x)));
        SET_COLOR(GET_P(x), BLACK);
        SET_COLOR(GET_L(w), BLACK);
        right_rotate(GET_P(x));
        x = ROOT;

      }
    }

  }

  SET_COLOR(x, BLACK);
}

/* replace node u with node v */
void red_black_transplant(struct node *u, struct node *v){
  if(GET_P(u) == NILL){
    ROOT = v;
  }
  else if(u == GET_L(GET_P(u))){
    SET_L(GET_P(u), v);
  }
  else{
    SET_R(GET_P(u), v);
  }

  SET_P(v, GET_P(u));
}
