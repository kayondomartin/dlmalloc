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
    if(prev == chunk){
        abort();
    }
    if(prev != 0){
        prev->prev_foot |= NEXT_EXH_BIT;
    }
    if(next !=0){
        next->prev_foot |= PREV_EXH_BIT;
    }
<<<<<<< HEAD
=======
    //return 0;
>>>>>>> 9f3bbd1c016552084985f4b723dfc92b89ed5b70
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

void split_and_unmap_segment(struct malloc_state* state, struct malloc_segment* segment, struct malloc_chunk* prev, struct malloc_chunk* next, char* split_start, size_t unmap_size){
    dl_assert(segment_holds(segment, split_start));
    dl_assert(segment_holds(segment, split_start+unmap_size));
    dl_assert(segment->base == split_start || segment_holds(segment, prev));

/*
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
*/
    char* unmap_end = split_start + unmap_size;
    //split or coalesce next_chunk
    if(unmap_end > next){//split_next
        size_t split_rem = unmap_end - (char*)next;
        size_t offset = align_offset(chunk_to_mem(++unmap_end));
        unmap_end--; 
    }
    char* prev_char = (char*)prev;
    char* segment_end = segment->base + segment->size;
    size_t ssize = pad_request(sizeof(struct malloc_segment));
    char* rawsp = segment_end - (ssize + sizeof(size_t)*4 + CHUNK_ALIGN_MASK);
    size_t offset = align_offset(chunk_to_mem(rawsp));
    char* asp = rawsp + offset;
    char* csp = (asp < (prev_char + MIN_CHUNK_SIZE))? prev_char: asp;
    if(segment->base == split_start){

    }
}
