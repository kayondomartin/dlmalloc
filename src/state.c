#include "assert.h"
#include "config.h"
#include "debug.h"
#include "init.h"
#include "lock.h"
#include "malloc.h"
#include "os.h"
#include "segment.h"
#include "state.h"
#include "log.h"
#include "check.h"

/* Initialize top chunk and its size */
void init_top(struct malloc_state *state, struct malloc_chunk *chunk, size_t size) {
    /* Ensure alignment */
    size_t offset = align_offset(chunk_to_mem(chunk));
    chunk = (struct malloc_chunk *) ((char *) chunk + offset);
    size -= offset;

    state->top = chunk;
    state->top_size = size;
    chunk->head = size | PREV_INUSE_BIT;
    /* set size of fake trailing chunk holding overhead space only once */
    chunk_plus_offset(chunk, size)->head = TOP_FOOT_SIZE;
    state->trim_check = params.trim_threshold; /* reset on each update */
}

/* Initialize bins for a new mstate that is otherwise zeroed out */
void init_bins(struct malloc_state *state) {
    /* Establish circular links for small_bins */
    for (bin_index_t i = 0; i < NUM_SMALL_BINS; ++i) {
        struct malloc_chunk *bin = small_bin_at(state, i);
        bin->fd = bin->bk = bin;
    }
}

/* Allocate chunk and prepend remainder with chunk in successor base. */
void *prepend_alloc(struct malloc_state *state, char *new_base, char *old_base, size_t nb) {
    struct malloc_chunk *p = align_as_chunk(new_base);
    struct malloc_chunk *oldfirst = align_as_chunk(old_base);
    size_t psize = (char *) oldfirst - (char *) p;
    struct malloc_chunk *q = chunk_plus_offset(p, nb);
    size_t qsize = psize - nb;
    set_size_and_prev_inuse_of_inuse_chunk(state, p, nb);

    dl_assert((char *) oldfirst > (char *) q);
    dl_assert(prev_inuse(oldfirst));
    dl_assert(qsize >= MIN_CHUNK_SIZE);

    /* consolidate remainder with first chunk of old base */
    if (oldfirst == state->top) {
        size_t tsize = state->top_size += qsize;
        state->top = q;
        q->head = tsize | PREV_INUSE_BIT;
        check_top_chunk(state, q);
    }
    else if (oldfirst == state->dv) {
        size_t dsize = state->dv_size += qsize;
        state->dv = q;
        set_size_and_prev_inuse_of_free_chunk(q, dsize);
    }
    else {
        if (!is_inuse(oldfirst)) {
            size_t nsize = chunk_size(oldfirst);
            unlink_chunk(state, oldfirst, nsize);
            oldfirst = chunk_plus_offset(oldfirst, nsize);
            qsize += nsize;
        }
        set_free_with_prev_inuse(q, qsize, oldfirst);
        insert_chunk(state, q, qsize);
        check_free_chunk(state, q);
    }

    check_malloced_chunk(state, chunk_to_mem(p), nb);
    return chunk_to_mem(p);
}

/* Add a segment to hold a new noncontiguous region */
void add_segment(struct malloc_state *state, char *tbase, size_t tsize, flag_t mmapped) {
    /* Determine locations and sizes of segment, fenceposts, old top */
    char *old_top = (char *) state->top;
    struct malloc_segment *oldsp = segment_holding(state, old_top);
    char *old_end = oldsp->base + oldsp->size;
    size_t ssize = pad_request(sizeof(struct malloc_segment));
    char *rawsp = old_end - (ssize + sizeof(size_t) * 4 + CHUNK_ALIGN_MASK);
    size_t offset = align_offset(chunk_to_mem(rawsp));
    char *asp = rawsp + offset;
    char *csp = (asp < (old_top + MIN_CHUNK_SIZE)) ? old_top : asp;
    struct malloc_chunk *sp = (struct malloc_chunk *) csp;
    struct malloc_segment *ss = (struct malloc_segment *) (chunk_to_mem(sp));
    struct malloc_chunk *tnext = chunk_plus_offset(sp, ssize);
    struct malloc_chunk *p = tnext;
    int nfences = 0;

    /* reset top to new space */
    init_top(state, (struct malloc_chunk *) tbase, tsize - TOP_FOOT_SIZE);

    /* Set up segment record */
    dl_assert(is_aligned(ss));
    set_size_and_prev_inuse_of_inuse_chunk(state, sp, ssize);
    *ss = state->segment; /* Push current record */
    state->segment.base = tbase;
    state->segment.size = tsize;
    state->segment.flags = mmapped;
    state->segment.next = ss;
    state->segment.blacklisted_size = 0;

    /* Insert trailing fenceposts */
    for (;;) {
        struct malloc_chunk *nextp = chunk_plus_offset(p, sizeof(size_t));
        p->head = FENCEPOST_HEAD;
        ++nfences;
        if ((char *) (&(nextp->head)) < old_end) {
            p = nextp;
        }
        else {
            break;
        }
    }
    dl_assert(nfences >= 2);

    /* Insert the rest of old top into a bin as an ordinary free chunk */
    if (csp != old_top) {
        struct malloc_chunk *q = (struct malloc_chunk *) old_top;
        size_t psize = csp - old_top;
        struct malloc_chunk *tn = chunk_plus_offset(q, psize);
        set_free_with_prev_inuse(q, psize, tn);
        insert_chunk(state, q, psize);
    }

    check_top_chunk(state, state->top);
}

int blacklist_chunk(struct malloc_state* state, struct malloc_chunk* chunk){
    set_chunk_tag(chunk, TAG_BITS);
    size_t csize = chunk_size(chunk);
    struct malloc_chunk *prev = is_prev_exhausted(chunk)? 0: chunk_minus_offset(chunk, (chunk->prev_foot & ~EXHAUSTION_BITS));
    struct malloc_chunk *next = is_next_exhausted(chunk)? 0: chunk_plus_offset(chunk, csize);
    //    if(prev == chunk){
    //    abort();
    //}
    if(prev && prev != chunk){
        prev->prev_foot |= NEXT_EXH_BIT;
    }
    if(next !=0){
        next->prev_foot |= PREV_EXH_BIT;
    }
    return invalidate_chunk(state, chunk);
}

void replace_segment(struct malloc_state *state, char *tbase, size_t tsize, flag_t mmapped, struct malloc_segment* pseg, struct malloc_segment* nseg){

    init_top(state, (struct malloc_chunk*) tbase, tsize - TOP_FOOT_SIZE);
    struct malloc_segment *ss;

    ss = &state->segment;
    state->segment.base = tbase;
    state->segment.size= tsize;
    state->segment.flags = mmapped;
    state->segment.blacklisted_size = 0;
    //state->top_colored_size = 0;
    //che
    if(pseg !=0){
        pseg->next = nseg;
        state->segment.next = ss;
    }else{
        state->segment.next = nseg;
    }
}

int blacklist_chunk2(struct malloc_state* state, struct malloc_chunk* chunk){
    dl_assert(is_exhausted(chunk));
    size_t size = chunk_size(chunk);
    struct malloc_segment* sh = segment_holding(state, chunk);
    sh->blacklisted_size += size;
    mte_color_tag(chunk, size, tag_to_int(TAG_BITS));
    size_t prev_size = chunk->prev_foot;
    if (ok_address(state, chunk) && ok_inuse(chunk)) {
        struct malloc_chunk* next = is_next_exhausted(chunk)? 0: chunk_plus_offset(chunk, size);
        struct malloc_chunk* prev = is_prev_exhausted(chunk)? 0: chunk_minus_offset(chunk, prev_size);
        if(prev != 0 && prev != chunk && !is_usable(prev) && prev != chunk){//coalesce backward
            dl_assert(is_inuse(prev));
            size += prev_size;
            chunk = prev;
            prev = chunk_minus_offset(chunk, chunk->prev_foot);
        }

        if(next == 0 || (ok_next(chunk,next) && ok_prev_inuse(next))){//coalesce forward
            if(next !=0 && !is_usable(next)){
                dl_assert(is_inuse(next));
                size += chunk_size(next);
                next = chunk_plus_offset(chunk, size);
            }

            chunk->head = size|TAG_BITS|chunk->head & FLAG_BITS;

            if((sh->size - sh->blacklisted_size - TOP_FOOT_SIZE) <= MIN_CHUNK_SIZE){//edited for debugging
                if(segment_holds(sh, state->dv)){
                    state->dv = 0;
                    state->dv_size = 0;
                }

                if(segment_holds(sh, state->top)){
                    struct malloc_chunk* nc = mem_to_chunk(sys_alloc(state, 0));
                    if(segment_holds(sh, nc)){
                        size_t page_size = params.page_size, page_offest = page_size-1;
                        char* unmap_begin = sh->base;
                        char* unmap_end = sh->base + (sh->size - page_size);
                        size_t unmap_size = (size_t)(unmap_end - unmap_size);
                        sh->base = unmap_end;
                        sh->size = page_size;
                        sh->blacklisted_size = 0;
                        if(state->least_addr == sh->base){
                            state->least_addr = unmap_end;
                        }
                        init_top(state, unmap_end, page_size-TOP_FOOT_SIZE);
                        return call_munmap(unmap_begin,unmap_size);
                    }else{
                        init_top(state, nc, nc->head);
                    }
                }
                unlink_segment(state, sh);
                return call_munmap(sh->base, sh->size);
                
            }

            if(size >= params.page_size){
                return try_chunk_unmap(state, sh, chunk, size);
            }
            
            finish:
            return 0;
        }

    }
    return -1;
}

int try_chunk_unmap(struct malloc_state* state, struct malloc_segment* sh, struct malloc_chunk* chunk, size_t size){
    dl_assert(!is_usable(chunk));
    size_t page_size = params.page_size;
    if(size >= page_size){
        size_t page_offset = page_size-1;
        char* unmap_base = (size_t)chunk & ~page_offset;

        if(unmap_base < (char*)chunk){
            unmap_base += page_size;
        }

        char* unmap_end = ((size_t)chunk+size) & ~page_offset;
        size_t unmap_size = unmap_end-unmap_base;
       
        if(unmap_size >= page_size){
            size_t prev_size = get_prev_size(chunk);
            struct malloc_chunk* next = is_next_exhausted(chunk)? 0: chunk_plus_offset(chunk, size);
            struct malloc_chunk* prev = is_prev_exhausted(chunk)? 0: chunk_minus_offset(chunk, prev_size);
            size_t rem_prev = unmap_base-(char*)chunk;
            if(rem_prev == 0  || rem_prev >= MIN_CHUNK_SIZE || (prev !=0 && !prev_inuse(chunk) && ((prev_size+rem_prev) >= MIN_CHUNK_SIZE))){
                size_t rem_next = next == 0? 0: (char*)next-unmap_end;
                if(rem_next != 0){
                    if(rem_next < MIN_CHUNK_SIZE){
                        if(curr_inuse(next)){
                            return 0;
                        }else{//recall to check next==top
                            size_t nsize = chunk_size(next);
                            unlink_chunk(state,next,nsize);
                            size_t bsize = MIN_CHUNK_SIZE-rem_next;
                            if(nsize-bsize < MIN_CHUNK_SIZE){//consume next
                                nsize += rem_next;
                                next = align_as_chunk(unmap_end);
                                next->prev_foot = PREV_EXH_BIT;
                                next->head = TAG_BITS|INUSE_BITS|nsize;
                                mte_color_tag(next, nsize, tag_to_int(TAG_BITS));
                            }else{//consume part of next
                                size_t ntag = get_chunk_tag(next);
                                nsize -= bsize;
                                next = align_as_chunk(unmap_end);
                                next->head = TAG_BITS|INUSE_BITS|MIN_CHUNK_SIZE;
                                mte_color_tag(next, MIN_CHUNK_SIZE, tag_to_int(TAG_BITS));
                                next->prev_foot = PREV_EXH_BIT;
                                next = chunk_plus_offset(next, MIN_CHUNK_SIZE);
                                next->head = nsize|ntag|PREV_INUSE_BIT;
                                next->prev_foot = MIN_CHUNK_SIZE;
                                insert_chunk(state, next, nsize);
                                next = chunk_minus_offset(next,MIN_CHUNK_SIZE);
                            }
                        }
                    }else{
                        next->prev_foot = rem_next;
                        next = align_as_chunk(unmap_end);
                        next->head = rem_next|TAG_BITS|INUSE_BITS;
                        next->prev_foot = PREV_EXH_BIT;
                    }
                }
                
                if(rem_prev !=0){
                    if(rem_prev < MIN_CHUNK_SIZE){
                        size_t rqsize = MIN_CHUNK_SIZE-rem_prev;
                        unlink_chunk(state, prev, prev_size);
                        if(prev_size-rqsize < MIN_CHUNK_SIZE){//consume prev
                            prev_size += rem_prev;
                            prev->prev_foot |= NEXT_EXH_BIT;
                            prev->head = prev_size|TAG_BITS|INUSE_BITS;
                            mte_color_tag(prev, prev_size, tag_to_int(TAG_BITS));
                        }else{//consume part of prev
                            size_t ptag_and_flags = prev->head & ~SIZE_BITS;
                            prev_size -= rqsize;
                            prev->head = prev_size|ptag_and_flags;
                            insert_chunk(state,prev,prev_size);
                            prev = chunk_plus_offset(prev, prev_size);
                            prev->head = MIN_CHUNK_SIZE|TAG_BITS|CURR_INUSE_BIT;
                            prev->prev_foot = prev_size|NEXT_EXH_BIT;
                            mte_color_tag(prev,MIN_CHUNK_SIZE,tag_to_int(TAG_BITS));
                        }
                    }else{
                        prev = chunk;
                        prev->head = rem_prev|TAG_BITS|(chunk->head & FLAG_BITS);
                        mte_color_tag(prev, rem_prev, tag_to_int(TAG_BITS));
                    }
                }

                if(unmap_base == sh->base){
                    sh->base = unmap_end;
                    sh->size -= unmap_size;
                    if(state->least_addr == unmap_base){
                        state->least_addr = unmap_end;
                    }
                }
                sh->blacklisted_size -= unmap_size;
                return call_munmap(unmap_base, unmap_size);
            }
        }
    }
    return 0;
}
