#include <stdlib.h>

#include "assert.h"
#include "check.h"
#include "chunk.h"
#include "config.h"
#include "debug.h"
#include "error.h"
#include "os.h"
#include "segment.h"
#include "state.h"

/* Relays to large vs small bin operations */
void insert_chunk(struct malloc_state *state, struct malloc_chunk *chunk, size_t size) {
    if (is_small(size)) {
        insert_small_chunk(state, chunk, size);
    }
    else {
        insert_large_chunk(state, (struct malloc_tree_chunk *) chunk, size);
    }
}

void unlink_chunk(struct malloc_state *state, struct malloc_chunk *chunk, size_t size) {
    if (is_small(size)) {
        unlink_small_chunk(state, chunk, size);
    }
    else {
        unlink_large_chunk(state, (struct malloc_tree_chunk *) chunk);
    }
}

/* Link a free chunk into a small_bin  */
void insert_small_chunk(struct malloc_state *state, struct malloc_chunk *chunk, size_t size) {
    bin_index_t index = small_index(size);
    struct malloc_chunk *b = small_bin_at(state, index);
    struct malloc_chunk *f = b;
    dl_assert(size >= MIN_CHUNK_SIZE);
    if (!small_map_is_marked(state, index)) {
        mark_small_map(state, index);
    }
    else if (likely(ok_address(state, b->fd))) {
        f = b->fd;
    }
    else {
        corruption_error(state);
    }
    b->fd = chunk;
    f->bk = chunk;
    chunk->fd = f;
    chunk->bk = b;
}

/* Unlink a chunk from a small_bin  */
void unlink_small_chunk(struct malloc_state *state, struct malloc_chunk *chunk, size_t size) {
    struct malloc_chunk *f = chunk->fd;
    struct malloc_chunk *b = chunk->bk;
    bin_index_t I = small_index(size);
    dl_assert(chunk != b);
    dl_assert(chunk != f);
    dl_assert(chunk_size(chunk) == small_index_to_size(I));
    if (likely(f == small_bin_at(state, I) || (ok_address(state, f) && f->bk == chunk))) {
        if (b == f) {
            clear_small_map(state, I);
        }
        else if (likely(b == small_bin_at(state, I) || (ok_address(state, b) && b->fd == chunk))) {
            f->bk = b;
            b->fd = f;
        }
        else {
            corruption_error(state);
        }
    }
    else {
        corruption_error(state);
    }
}

/* Unlink the first chunk from a small_bin */
void unlink_first_small_chunk(struct malloc_state *state, struct malloc_chunk *b, struct malloc_chunk *chunk, bin_index_t index) {
    struct malloc_chunk *f = chunk->fd;
    dl_assert(chunk != b);
    dl_assert(chunk != f);
    dl_assert(chunk_size(chunk) == small_index_to_size(index));
    if (b == f) {
        clear_small_map(state, index);
    }
    else if (likely(ok_address(state, f) && f->bk == chunk)) {
        f->bk = b;
        b->fd = f;
    }
    else {
        corruption_error(state);
    }
}

/* Replace dv node, binning the old one */
/* Used only when dv_size known to be small */
void replace_dv(struct malloc_state *state, struct malloc_chunk *chunk, size_t size) {
    size_t dv_size = state->dv_size;
    dl_assert(is_small(dv_size));
    if (dv_size != 0) {
        insert_small_chunk(state, state->dv, dv_size);
    }
    state->dv_size = size;
    state->dv = chunk;
}

/* Insert chunk into tree */
void insert_large_chunk(struct malloc_state *state, struct malloc_tree_chunk *chunk, size_t size) {
    struct malloc_tree_chunk **H;
    bin_index_t I;
    compute_tree_index(size, I);
    H = tree_bin_at(state, I);
    chunk->index = I;
    chunk->child[0] = chunk->child[1] = 0;
    if (!tree_map_is_marked(state, I)) {
        mark_tree_map(state, I);
        *H = chunk;
        chunk->parent = (struct malloc_tree_chunk *) H;
        chunk->fd = chunk->bk = chunk;
    }
    else {
        struct malloc_tree_chunk *T = *H;
        size_t K = size << leftshift_for_tree_index(I);
        for (;;) {
            if (chunk_size(T) != size) {
                struct malloc_tree_chunk **C = &(T->child[(K >> (SIZE_T_BITSIZE - (size_t) 1)) & 1]);
                K <<= 1;
                if (*C != 0) {
                    T = *C;
                }
                else if (likely(ok_address(state, C))) {
                    *C = chunk;
                    chunk->parent = T;
                    chunk->fd = chunk->bk = chunk;
                    break;
                }
                else {
                    corruption_error(state);
                    break;
                }
            }
            else {
                struct malloc_tree_chunk *F = T->fd;
                if (likely(ok_address(state, T) && ok_address(state, F))) {
                    T->fd = F->bk = chunk;
                    chunk->fd = F;
                    chunk->bk = T;
                    chunk->parent = 0;
                    break;
                }
                else {
                    corruption_error(state);
                    break;
                }
            }
        }
    }
}

/*
  Unlink steps:

  1. If x is a chained node, unlink it from its same-sized fd/bk links
     and choose its bk node as its replacement.
  2. If x was the last node of its size, but not a leaf node, it must
     be replaced with a leaf node (not merely one with an open left or
     right), to make sure that lefts and rights of descendents
     correspond properly to bit masks.  We use the rightmost descendent
     of x.  We could use any other leaf, but this is easy to locate and
     tends to counteract removal of leftmosts elsewhere, and so keeps
     paths shorter than minimally guaranteed.  This doesn't loop much
     because on average a node in a tree is near the bottom.
  3. If x is the base of a chain (i.e., has parent links) relink
     x's parent and children to x's replacement (or null if none).
*/
void unlink_large_chunk(struct malloc_state *state, struct malloc_tree_chunk *chunk) {
    struct malloc_tree_chunk *XP = chunk->parent;
    struct malloc_tree_chunk *R;
    if (chunk->bk != chunk) {
        struct malloc_tree_chunk *F = chunk->fd;
        R = chunk->bk;
        if (likely(ok_address(state, F) && F->bk == chunk && R->fd == chunk)) {
            F->bk = R;
            R->fd = F;
        }
        else {
            corruption_error(state);
        }
    }
    else {
        struct malloc_tree_chunk **RP;
        if (((R = *(RP = &(chunk->child[1]))) != 0) || ((R = *(RP = &(chunk->child[0]))) != 0)) {
            struct malloc_tree_chunk **CP;
            while ((*(CP = &(R->child[1])) != 0) || (*(CP = &(R->child[0])) != 0)) {
                R = *(RP = CP);
            }
            if (likely(ok_address(state, RP))) {
                *RP = 0;
            }
            else {
                corruption_error(state);
            }
        }
    }
    if (XP != 0) {
        struct malloc_tree_chunk **H = tree_bin_at(state, chunk->index);
        if (chunk == *H) {
            if ((*H = R) == 0) {
                clear_tree_map(state, chunk->index);
            }
        }
        else if (likely(ok_address(state, XP))) {
            if (XP->child[0] == chunk) {
                XP->child[0] = R;
            }
            else {
                XP->child[1] = R;
            }
        }
        else {
            corruption_error(state);
        }
        if (R != 0) {
            if (likely(ok_address(state, R))) {
                struct malloc_tree_chunk *C0, *C1;
                R->parent = XP;
                if ((C0 = chunk->child[0]) != 0) {
                    if (likely(ok_address(state, C0))) {
                        R->child[0] = C0;
                        C0->parent = R;
                    }
                    else {
                        corruption_error(state);
                    }
                }
                if ((C1 = chunk->child[1]) != 0) {
                    if (likely(ok_address(state, C1))) {
                        R->child[1] = C1;
                        C1->parent = R;
                    }
                    else {
                        corruption_error(state);
                    }
                }
            }
            else {
                corruption_error(state);
            }
        }
    }
}

/* Consolidate and bin a chunk. Differs from exported versions
   of free mainly in that the chunk need not be marked as inuse.
*/

void  dispose_chunk(struct malloc_state *state, struct malloc_chunk *chunk, size_t size) {

  int free_state = 0;
  size_t new_tag = get_chunk_tag(chunk) + TAG_OFFSET;

  if(new_tag == TAG_BITS){
    blacklist_chunk(state, chunk);
    return;
  }
  int consolidation = 1;
#if DISABLE_CONSOLIDATION
  consolidation = 0;
#endif

  struct malloc_chunk* base = chunk;
  size_t csize = size;
  struct malloc_chunk *next = is_next_exhausted(chunk)? 0: chunk_plus_offset(chunk, size);
  if (!prev_inuse(chunk)) {
    size_t prev_size = get_prev_size(chunk);
    if (is_mmapped(chunk)) {
      size += prev_size + MMAP_FOOT_PAD;
      if (call_munmap((char *) chunk - prev_size, size) == 0) {
        state->footprint -= size;
      }
      return;
    }
    struct malloc_chunk *prev = is_prev_exhausted(chunk)? 0: chunk_minus_offset(chunk, prev_size);

    if ( consolidation && prev != 0 && likely(ok_address(state, prev))) { /* consolidate backward */
      size_t prev_tag = get_chunk_tag(prev);
#if ANAYZE_NOMAD
      if(new_tag > prev_tag){
        if(new_tag-prev_tag > TAG_DISPLACEMENT)
          goto LABEL0;
      }
      else{
        if(prev_tag-new_tag > TAG_DISPLACEMENT)
          goto LABEL0;
      }
#endif

      new_tag = tag_max(new_tag, get_chunk_tag(prev));
      size += prev_size;
      chunk = prev;
      if (prev != state->dv) {
        unlink_chunk(state, prev, prev_size);
      }
      else if (next == 0 || ((next->head & INUSE_BITS) == INUSE_BITS)) {
        set_chunk_tag(chunk, new_tag);
        if(prev_tag == new_tag){//color chunk only
          mte_color_tag(base, csize, tag_to_int(new_tag));
        }else{//color both chunk and prev
          mte_color_tag(chunk, size, tag_to_int(new_tag));
        }

        state->dv_size = size;
        chunk->head = size| new_tag | PREV_INUSE_BIT;
        if(next == 0){
          chunk->prev_foot |= NEXT_EXH_BIT;
        }else{
          next->prev_foot = (next->prev_foot & NEXT_EXH_BIT) | size;
          next->head &= ~PREV_INUSE_BIT;
        }
      }
    LABEL0:
    }
    else if(!consolidation){
      corruption_error(state);
      return;
    }
  }
  if (next == 0 || likely(ok_address(state, next))) {
    if (consolidation && next !=0 && !curr_inuse(next)) {  /* consolidate forward */

      size_t next_tag = get_chunk_tag(next);
#if ANAYZE_NOMAD
      if(new_tag > next_tag){
        if(new_tag-next_tag > TAG_DISPLACEMENT)
          goto LABEL1;
      }
      else{
        if(next_tag-new_tag > TAG_DISPLACEMENT)
          goto LABEL1;
      }
#endif

      size_t nsize = chunk_size(next);
      if(next_tag > new_tag){
        new_tag = next_tag;
      }else if(new_tag != next_tag){
        set_chunk_tag(next, new_tag);
      }

      if(!curr_inuse(chunk)){
        size_t prev_tag = get_chunk_tag(chunk);
        set_chunk_tag(base, new_tag);
        if(prev_tag == next_tag && next_tag == new_tag){//color only chunk
          mte_color_tag(base, csize, tag_to_int(new_tag));
        }else if(prev_tag == new_tag){//color next and chunk
          if(next == state->top){
            mte_color_tag(base, state->top_colored_size+csize, tag_to_int(new_tag));
          }else{
            mte_color_tag(base, csize+nsize, tag_to_int(new_tag));
          }
        }else if(new_tag == next_tag){//color prev and p
          mte_color_tag(chunk, size, tag_to_int(new_tag));
        }else {//color prev, curr, next
          if(next == state->top){
            mte_color_tag(chunk, state->top_colored_size + size, tag_to_int(new_tag));
          }else{
            mte_color_tag(chunk, size+nsize, tag_to_int(new_tag));
          }
        }
        set_chunk_tag(base, new_tag);
      }else{
        if(new_tag == next_tag){
          mte_color_tag(chunk, size, tag_to_int(new_tag));
        }else if(next == state->top){
          mte_color_tag(chunk, state->top_colored_size + size, tag_to_int(new_tag));
        }else{
          mte_color_tag(chunk, size+nsize, tag_to_int(new_tag));
        }
      }

      if (next == state->top) {
        size_t tsize = state->top_size += size;
        state->top_colored_size += size;
        state->top = chunk;
        chunk->head = tsize | PREV_INUSE_BIT | new_tag;
        if (chunk == state->dv) {
          state->dv = 0;
          state->dv_size = 0;
        }
        return;
      }
      else if (next == state->dv) {
        size_t dsize = state->dv_size += size;
        state->dv = chunk;
        chunk->prev_foot |= (next->prev_foot & NEXT_EXH_BIT);
        set_size_and_prev_inuse_of_free_chunk(chunk, dsize);
        set_chunk_tag(chunk, new_tag); 
        return;
      }
      else {
        size += nsize;
        chunk->prev_foot |= (next->prev_foot & NEXT_EXH_BIT);
        unlink_chunk(state, next, nsize);
        set_size_and_prev_inuse_of_free_chunk(chunk, size);
        set_chunk_tag(chunk, new_tag);
        if (chunk == state->dv) {
          state->dv_size = size;
          return;
        }
      }
    }
    else {
      if(!curr_inuse(chunk)){
        size_t prev_tag = get_chunk_tag(chunk);
        if(new_tag == prev_tag){
          mte_color_tag(base, csize, tag_to_int(new_tag));
        }else{
          mte_color_tag(chunk, size, tag_to_int(new_tag));
        }
        set_chunk_tag(base, new_tag);
      }else{
        mte_color_tag(chunk, size, tag_to_int(new_tag));
      }
      chunk->head = new_tag | size | PREV_INUSE_BIT;
      if(next ==0){
        chunk->prev_foot |= NEXT_EXH_BIT;
      }else{
        next->prev_foot = (next->prev_foot & NEXT_EXH_BIT)|size;
        next->head &= ~PREV_INUSE_BIT;
      }
    }
  LABEL1:
    insert_chunk(state, chunk, size);
  }
  else {
    corruption_error(state);
  }
}
