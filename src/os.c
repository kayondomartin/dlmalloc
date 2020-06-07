#include <sys/types.h>

#include "assert.h"
#include "chunk.h"
#include "config.h"
#include "debug.h"
#include "error.h"
#include "init.h"
#include "os.h"

/* For sys_alloc, enough padding to ensure can malloc request on success */
#define SYS_ALLOC_PADDING       (TOP_FOOT_SIZE + MALLOC_ALIGNMENT)

#define HALF_MAX_SIZE_T         (MAX_SIZE_T / 2U)

/* -----------------------  Direct-mmapping chunks ----------------------- */

/*
  Directly mmapped chunks are set up with an offset to the start of
  the mmapped region stored in the prev_foot field of the chunk. This
  allows reconstruction of the required argument to MUNMAP when freed,
  and also allows adjustment of the returned chunk to meet alignment
  requirements (especially in memalign).
*/

/* Malloc using mmap. */
void *mmap_alloc(struct malloc_state *state, size_t size) {
    size_t mm_size = mmap_align(size + sizeof(size_t) * 6 + CHUNK_ALIGN_MASK);
    if (state->footprint_limit != 0) {
        size_t footprint = state->footprint + mm_size;
        if (footprint <= state->footprint || footprint > state->footprint_limit) {
            return 0;
        }
    }
    if (mm_size > size) {     /* Check for wrap around 0 */
        char *mm = (char *) call_mmap(mm_size);
        if (mm != MFAIL) {
            size_t offset = align_offset(chunk_to_mem(mm));
            size_t p_size = mm_size - offset - MMAP_FOOT_PAD;
            struct malloc_chunk *p = (struct malloc_chunk *) (mm + offset);
            p->prev_foot = offset;
            p->head = p_size;
            mark_inuse_foot(state, p, p_size);
            chunk_plus_offset(p, p_size)->head = FENCEPOST_HEAD;
            chunk_plus_offset(p, p_size + sizeof(size_t))->head = 0;
            if (state->least_addr == 0 || mm < state->least_addr) {
                state->least_addr = mm;
            }
            if ((state->footprint += mm_size) > state->max_footprint) {
                state->max_footprint = state->footprint;
            }
            dl_assert(is_aligned(chunk_to_mem(p)));
            check_mmapped_chunk(state, p);
            return chunk_to_mem(p);
        }
    }
    return 0;
}

/* Realloc using mmap. */
struct malloc_chunk *mmap_resize(struct malloc_state *state, struct malloc_chunk *old_p, size_t size, int flags) {
    size_t old_size = chunk_size(old_p);
    if (is_small(size)) {
        return 0; /* Can't shrink mmap regions below small size. */
    }
    if (old_size >= size + sizeof(size_t) && (old_size - size) <= (params.granularity << 1)) {
        return old_p; /* Keep old chunk if big enough but not too big. */
    }
    else {
        size_t offset = old_p->prev_foot;
        size_t old_mm_size = old_size + offset + MMAP_FOOT_PAD;
        size_t new_mm_size = mmap_align(size + sizeof(size_t) * 6 + CHUNK_ALIGN_MASK);
        char *cp = (char *) call_mremap((char *) old_p - offset, old_mm_size, new_mm_size, flags);
        if (cp != MFAIL) {
            struct malloc_chunk *new_p = (struct malloc_chunk *) (cp + offset);
            size_t p_size = new_mm_size - offset - MMAP_FOOT_PAD;
            new_p->head = p_size;
            mark_inuse_foot(state, new_p, p_size);
            chunk_plus_offset(new_p, p_size)->head = FENCEPOST_HEAD;
            chunk_plus_offset(new_p, p_size + sizeof(size_t))->head = 0;
            if (cp < state->least_addr) {
                state->least_addr = cp;
            }
            if ((state->footprint += new_mm_size - old_mm_size) > state->max_footprint) {
                state->max_footprint = state->footprint;
            }
            check_mmapped_chunk(state, new_p);
            return new_p;
        }
    }
    return 0;
}

/* -------------------------- System allocation -------------------------- */

/* Get memory from system using MORECORE or MMAP */
void *sys_alloc(struct malloc_state *state, size_t size) {
    ensure_initialization();

    /* Directly map large chunks, but only if already initialized */
    if (use_mmap(state) && size >= params.mmap_threshold && state->top_size != 0) {
        void *mem = mmap_alloc(state, size);
        if (mem != 0) {
            return mem;
        }
    }

    size_t alloc_size = granularity_align(size + SYS_ALLOC_PADDING);
    if (alloc_size <= size) {
        return 0; /* wraparound */
    }
    if (state->footprint_limit != 0) {
        size_t footprint = state->footprint + alloc_size;
        if (footprint <= state->footprint || footprint > state->footprint_limit) {
            return 0;
        }
    }

    char *tbase = MFAIL;
    size_t tsize = 0;
    flag_t mmap_flag = 0;

    /*
      Try getting memory in any of three ways (in most-preferred to
      least-preferred order):
      1. A call to MORECORE that can normally contiguously extend memory.
         (disabled if not MORECORE_CONTIGUOUS or not HAVE_MORECORE or
         or main space is mmapped or a previous contiguous call failed)
      2. A call to MMAP new space (disabled if not HAVE_MMAP).
         Note that under the default settings, if MORECORE is unable to
         fulfill a request, and HAVE_MMAP is true, then mmap is
         used as a noncontiguous system allocator. This is a useful backup
         strategy for systems with holes in address spaces -- in this case
         sbrk cannot contiguously expand the heap, but mmap may be able to
         find space.
      3. A call to MORECORE that cannot usually contiguously extend memory.
         (disabled if not HAVE_MORECORE)

     In all cases, we need to request enough bytes from system to ensure
     we can malloc size bytes upon success, so pad with enough space for
     top_foot, plus alignment-pad to make sure we don't lose bytes if
     not on boundary, and round this up to a granularity unit.
    */

    if (MORECORE_CONTIGUOUS && !use_noncontiguous(state)) {
        char *br = MFAIL;
        size_t ssize = alloc_size; /* sbrk call size */
        struct malloc_segment *ss = (state->top == 0) ? 0 : segment_holding(state, (char *) state->top);
        ACQUIRE_MALLOC_GLOBAL_LOCK();

        if (ss == 0) {  /* First time through or recovery */
            char *base = (char *) call_sbrk(0);
            if (base != MFAIL) {
                /* Adjust to end on a page boundary */
                if (!is_page_aligned(base)) {
                    ssize += page_align((size_t) base) - (size_t) base;
                }
                size_t footprint = state->footprint + ssize;                 /* recheck limits */
                if (ssize > size
                    && ssize < HALF_MAX_SIZE_T
                    && (state->footprint_limit == 0 || (footprint > state->footprint && footprint <= state->footprint_limit))
                    && (br = (char *) call_sbrk(ssize)) == base) {
                    tbase = base;
                    tsize = ssize;
                }
            }
        }
        else {
            /* Subtract out existing available top space from MORECORE request. */
            ssize = granularity_align(size - state->top_size + SYS_ALLOC_PADDING);
            /* Use mem here only if it did continuously extend old space */
            if (ssize < HALF_MAX_SIZE_T && (br = (char *) call_sbrk(ssize)) == ss->base + ss->size) {
                tbase = br;
                tsize = ssize;
            }
        }

        if (tbase == MFAIL) {    /* Cope with partial failure */
            if (br != MFAIL) {    /* Try to use/extend the space we did get */
                if (ssize < HALF_MAX_SIZE_T && ssize < size + SYS_ALLOC_PADDING) {
                    size_t esize = granularity_align(size + SYS_ALLOC_PADDING - ssize);
                    if (esize < HALF_MAX_SIZE_T) {
                        char *end = (char *) call_sbrk(esize);
                        if (end != MFAIL) {
                            ssize += esize;
                        }
                        else {            /* Can't use; try to release */
                            (void) call_sbrk(-ssize);
                            br = MFAIL;
                        }
                    }
                }
            }
            if (br != MFAIL) {    /* Use the space we did get */
                tbase = br;
                tsize = ssize;
            }
            else {
                disable_contiguous(state); /* Don't try contiguous path in the future */
            }
        }

        RELEASE_MALLOC_GLOBAL_LOCK();
    }

    if (tbase == MFAIL) {  /* Try MMAP */
        char *mp = (char *) call_mmap(alloc_size);
        if (mp != MFAIL) {
            tbase = mp;
            tsize = alloc_size;
            mmap_flag = USE_MMAP_BIT;
        }
    }

    if (tbase == MFAIL) { /* Try noncontiguous MORECORE */
        if (alloc_size < HALF_MAX_SIZE_T) {
            char *br = MFAIL;
            char *end = MFAIL;
            ACQUIRE_MALLOC_GLOBAL_LOCK();
            br = (char *) call_sbrk(alloc_size);
            end = (char *) call_sbrk(0);
            RELEASE_MALLOC_GLOBAL_LOCK();
            if (br != MFAIL && end != MFAIL && br < end) {
                size_t ssize = end - br;
                if (ssize > size + TOP_FOOT_SIZE) {
                    tbase = br;
                    tsize = ssize;
                }
            }
        }
    }

    if (tbase != MFAIL) {
        if ((state->footprint += tsize) > state->max_footprint) {
            state->max_footprint = state->footprint;
        }

        if (!is_initialized(state)) { /* first-time initialization */
            if (state->least_addr == 0 || tbase < state->least_addr) {
                state->least_addr = tbase;
            }
            state->segment.base = tbase;
            state->segment.size = tsize;
            state->segment.flags = mmap_flag;
            state->magic = params.magic;
            state->release_checks = MAX_RELEASE_CHECK_RATE;
            init_bins(state);
            if (is_global(state)) {
                init_top(state, (struct malloc_chunk *) tbase, tsize - TOP_FOOT_SIZE);
            }
            else {
                /* Offset top by embedded malloc_state */
                struct malloc_chunk *mn = next_chunk(mem_to_chunk(state));
                init_top(state, mn, (size_t) ((tbase + tsize) - (char *) mn) - TOP_FOOT_SIZE);
            }
        }
        else {
            /* Try to merge with an existing segment */
            struct malloc_segment *sp = &state->segment;
            /* Only consider most recent segment if traversal suppressed */
            while (sp != 0 && tbase != sp->base + sp->size) {
                sp = sp->next;
            }
            if (sp != 0 //tmte mark: contigous segments
                && !is_extern_segment(sp) //was allocated here
                && (sp->flags & USE_MMAP_BIT) == mmap_flag //it's allocated same way as this one
                && segment_holds(sp, state->top)) { /* append */
                sp->size += tsize;
                init_top(state, state->top, state->top_size + tsize);
            }
            else {
                if (tbase < state->least_addr) {
                    state->least_addr = tbase;
                }
                sp = &state->segment;
                while (sp != 0 && sp->base != tbase + tsize) {
                    sp = sp->next;
                }
                if (sp != 0 && !is_extern_segment(sp) && (sp->flags & USE_MMAP_BIT) == mmap_flag) {
                    char *oldbase = sp->base;
                    sp->base = tbase;
                    sp->size += tsize;
                    return prepend_alloc(state, tbase, oldbase, size);
                }
                else {
                    add_segment(state, tbase, tsize, mmap_flag);
                }
            }
        }

        if (size < state->top_size) { /* Allocate from new or extended top space */
            size_t rsize = state->top_size -= size;
            struct malloc_chunk *p = state->top;
            struct malloc_chunk *r = state->top = chunk_plus_offset(p, size);
            r->head = rsize | PREV_INUSE_BIT;
            set_size_and_prev_inuse_of_inuse_chunk(state, p, size);
            check_top_chunk(state, state->top);
            check_malloced_chunk(state, chunk_to_mem(p), size);
            return chunk_to_mem(p);
        }
    }
    malloc_failure();
    return 0;
}

/* -----------------------  system deallocation -------------------------- */

int sys_trim(struct malloc_state *state, size_t pad) {
    ensure_initialization();

    size_t released = 0;
    if (is_initialized(state) && pad < MAX_REQUEST) {
        pad += TOP_FOOT_SIZE; /* ensure enough room for segment overhead */

        if (state->top_size > pad) {
            /* Shrink top space in granularity-size units, keeping at least one */
            size_t unit = params.granularity;
            size_t extra = ((state->top_size - pad + (unit - (size_t) 1)) / unit - (size_t) 1) * unit;
            struct malloc_segment *segment = segment_holding(state, (char *) state->top);

            if (!is_extern_segment(segment)) {
                if (is_mmapped_segment(segment)) {
                    if (segment->size >= extra && !has_segment_link(state, segment)) { /* can't shrink if pinned */
                        size_t new_size = segment->size - extra;
                        /* Prefer mremap, fall back to munmap */
                        if (call_mremap(segment->base, segment->size, new_size, 0) != MFAIL
                            || call_munmap(segment->base + new_size, extra) == 0) {
                            released = extra;
                        }
                    }
                }
                else {
                    if (extra >= HALF_MAX_SIZE_T) { /* Avoid wrapping negative */
                        extra = HALF_MAX_SIZE_T + (size_t) 1 - unit;
                    }
                    ACQUIRE_MALLOC_GLOBAL_LOCK();
                    {
                        /* Make sure end of memory is where we last set it. */
                        char *old_br = (char *) call_sbrk(0);
                        if (old_br == segment->base + segment->size) {
                            char *rel_br = (char *) call_sbrk(-extra);
                            char *new_br = (char *) call_sbrk(0);
                            if (rel_br != MFAIL && new_br < old_br) {
                                released = old_br - new_br;
                            }
                        }
                    }
                    RELEASE_MALLOC_GLOBAL_LOCK();
                }
            }

            if (released != 0) {
                segment->size -= released;
                state->footprint -= released;
                init_top(state, state->top, state->top_size - released);
                check_top_chunk(state, state->top);
            }
        }

        /* Unmap any unused mmapped segments */
        released += release_unused_segments(state);

        /* On failure, disable auto-trim to avoid repeated failed future calls */
        if (released == 0 && state->top_size > state->trim_check) {
            state->trim_check = MAX_SIZE_T;
        }
    }

    return released != 0 ? 1 : 0;
}

/* Unmap and unlink any mmapped segments that don't contain used chunks */
size_t release_unused_segments(struct malloc_state *state) {
    size_t released = 0;
    int num_segments = 0;
    struct malloc_segment *prev = &state->segment;
    struct malloc_segment *segment = prev->next;
    while (segment != 0) {
        char *base = segment->base;
        size_t size = segment->size;
        struct malloc_segment *next = segment->next;
        ++num_segments;
        if (is_mmapped_segment(segment) && !is_extern_segment(segment)) {
            struct malloc_chunk *p = align_as_chunk(base);
            size_t p_size = chunk_size(p);
            /* Can unmap if first chunk holds entire segment and not pinned */
            if (!is_inuse(p) && (char *) p + p_size >= base + size - TOP_FOOT_SIZE) {
                struct malloc_tree_chunk *tp = (struct malloc_tree_chunk *) p;
                dl_assert(segment_holds(segment, (char *) segment));
                if (p == state->dv) {
                    state->dv = 0;
                    state->dv_size = 0;
                }
                else {
                    unlink_large_chunk(state, tp);
                }
                if (call_munmap(base, size) == 0) {
                    released += size;
                    state->footprint -= size;
                    /* unlink obsoleted record */
                    segment = prev;
                    segment->next = next;
                }
                else { /* back out if cannot unmap */
                    insert_large_chunk(state, tp, p_size);
                }
            }
        }
        prev = segment;
        segment = next;
    }
    /* Reset check counter */
    state->release_checks = (size_t) num_segments > (size_t) MAX_RELEASE_CHECK_RATE
                            ? (size_t) num_segments
                            : (size_t) MAX_RELEASE_CHECK_RATE;
    return released;
}

/* tmte edit: ops definitions */
void replace_released_segment(struct malloc_state* state, struct malloc_segment* pseg, struct malloc_segment* nseg){

    size_t alloc_size = granularity_align(SYS_ALLOC_PADDING);
    if (state->footprint_limit != 0) {
        size_t footprint = state->footprint + alloc_size;
        if (footprint <= state->footprint || footprint > state->footprint_limit) {
            return 0;
        }
    }

    char *tbase = MFAIL;
    size_t tsize = 0;
    flag_t mmap_flag = 0;

    if (MORECORE_CONTIGUOUS && use_noncontiguous(state)) {
        char *br = MFAIL;
        size_t ssize = alloc_size; /* sbrk call size */
        ACQUIRE_MALLOC_GLOBAL_LOCK();

        char *base = (char *) call_sbrk(0);
        if (base != MFAIL) {
            /* Adjust to end on a page boundary */
            if (!is_page_aligned(base)) {
                ssize += page_align((size_t) base) - (size_t) base;
            }
            size_t footprint = state->footprint + ssize;                 /* recheck limits */
            if (ssize > 0
                && ssize < HALF_MAX_SIZE_T
                && (state->footprint_limit == 0 || (footprint > state->footprint && footprint <= state->footprint_limit))
                && (br = (char *) call_sbrk(ssize)) == base) {
                tbase = base;
                tsize = ssize;
            }
        }

        if (tbase == MFAIL) {    /* Cope with partial failure */
            if (br != MFAIL) {    /* Try to use/extend the space we did get */
                if (ssize < HALF_MAX_SIZE_T && ssize < SYS_ALLOC_PADDING) {
                    size_t esize = granularity_align(SYS_ALLOC_PADDING - ssize);
                    if (esize < HALF_MAX_SIZE_T) {
                        char *end = (char *) call_sbrk(esize);
                        if (end != MFAIL) {
                            ssize += esize;
                        }
                        else {            /* Can't use; try to release */
                            (void) call_sbrk(-ssize);
                            br = MFAIL;
                        }
                    }
                }
            }
            if (br != MFAIL) {    /* Use the space we did get */
                tbase = br;
                tsize = ssize;
            }
            else {
                disable_contiguous(state); /* Don't try contiguous path in the future */
            }
        }

        RELEASE_MALLOC_GLOBAL_LOCK();
    }

    if (tbase == MFAIL) {  /* Try MMAP */
        char *mp = (char *) call_mmap(alloc_size);
        if (mp != MFAIL) {
            tbase = mp;
            tsize = alloc_size;
            mmap_flag = USE_MMAP_BIT;
        }
    }

    if (tbase == MFAIL) { /* Try noncontiguous MORECORE */
        if (alloc_size < HALF_MAX_SIZE_T) {
            char *br = MFAIL;
            char *end = MFAIL;
            ACQUIRE_MALLOC_GLOBAL_LOCK();
            br = (char *) call_sbrk(alloc_size);
            end = (char *) call_sbrk(0);
            RELEASE_MALLOC_GLOBAL_LOCK();
            if (br != MFAIL && end != MFAIL && br < end) {
                size_t ssize = end - br;
                if (ssize > TOP_FOOT_SIZE) {
                    tbase = br;
                    tsize = ssize;
                }
            }
        }
    }

    if (tbase != MFAIL) {
        if ((state->footprint += tsize) > state->max_footprint) {
            state->max_footprint = state->footprint;
        }

        if (!is_initialized(state)) { /* first-time initialization */
            if (state->least_addr == 0 || tbase < state->least_addr) {
                state->least_addr = tbase;
            }
            state->segment.base = tbase;
            state->segment.size = tsize;
            state->segment.blacklisted_size = 0;
            state->segment.flags = mmap_flag;
            state->magic = params.magic;
            state->release_checks = MAX_RELEASE_CHECK_RATE;
            init_bins(state);
            if (is_global(state)) {
                init_top(state, (struct malloc_chunk *) tbase, tsize - TOP_FOOT_SIZE);
            }
            else {
                /* Offset top by embedded malloc_state */
                struct malloc_chunk *mn = next_chunk(mem_to_chunk(state));
                init_top(state, mn, (size_t) ((tbase + tsize) - (char *) mn) - TOP_FOOT_SIZE);
            }
        }
        else {
            
            if (tbase < state->least_addr) {
                state->least_addr = tbase;
            }
            replace_segment(state, tbase, tsize,mmap_flag,pseg, nseg);
        }
        return;
    }
    malloc_failure();
    return 0;
}


void release_exhausted_segment(struct malloc_state *state, struct malloc_segment* segment){
    size_t size  = segment->size;
    size_t base = segment->base;

    struct malloc_segment* pseg = prev_segment(state, segment);
    struct malloc_segment* nseg = segment->next;

    //check least_addr
    if(state->least_addr == segment->base){
        struct malloc_segment* s = &state->segment;
        char* new_base = s->base;
        for(;;){
            if(s->base < segment->base && s != segment){
                new_base = s->base;
            }
            if(s = s->next == 0){
                break;
            }
        }
        state->least_addr = new_base;
    }
    
    if(segment_holds(segment, state->top)){
        if(pseg == 0 && nseg == 0){//only one segment 
            state->top = 0;
            state->top_size = 0;
            state->top_colored_size = 0;
            state->least_addr = 0;
        }
        replace_released_segment(state, pseg, nseg);
    }else{

        if(segment_holds(segment, state->dv)){
            state->dv = 0;
            state->dv_size = 0;
        }

        if(pseg != 0 && nseg != 0){
            pseg->next = nseg;
        }else if(pseg != 0){
            pseg->next = 0;
        }else if(nseg !=0){
            state->segment = *nseg;
        }

        call_munmap(segment->base, segment->size);
    }
}

int release_exhausted_chunk(struct malloc_state* state, struct malloc_chunk* chunk, struct malloc_chunk* prev, struct malloc_chunk* next, size_t size){
    size_t page_size = params.page_size;
    size_t offset = page_size-1;
    char* unmap_base = (size_t)chunk & ~offset;
    unmap_base = unmap_base < (char*)chunk? unmap_base+page_size: unmap_base;
    char* unmap_end = (size_t)((char*)chunk + size) & ~offset;
    size_t unmap_size = unmap_end-unmap_base;
    size_t remainder_p = unmap_base-(char*)chunk;
    size_t remainder_n = ((char*)chunk+size)-unmap_end;

    if(unmap_size < page_size){
        return 0;
    }

    struct malloc_chunk* new_prev = prev;
    struct malloc_chunk* rem_chunk;
    if(remainder_p !=0){
        if(remainder_p < MIN_CHUNK_SIZE && curr_inuse(prev)){
            if(remainder_n ==0 || remainder_n >= MIN_CHUNK_SIZE){
                rem_chunk = align_as_chunk(unmap_end);
                rem_chunk->head = TAG_BITS| remainder_n| INUSE_BITS;
                chunk->head = chunk->head & ~SIZE_BITS| chunk_size(chunk)-remainder_n;
                rem_chunk->prev_foot = unmap_size;
            }else if(remainder_n !=0 && !curr_inuse(next)){
                size_t next_size = chunk_size(next), next_tag = get_chunk_tag(next);
                unlink_chunk(state, next, next_size);
                chunk->head = chunk->head & ~SIZE_BITS| chunk_size(chunk)-remainder_n;
                next_size = next_size - (MIN_CHUNK_SIZE-remainder_n);
                remainder_n = MIN_CHUNK_SIZE;
                rem_chunk = align_as_chunk(unmap_end);
                rem_chunk->head = MIN_CHUNK_SIZE | TAG_BITS| INUSE_BITS| BLACKLIST_BIT;
                rem_chunk->prev_foot = unmap_size;
                next = rem_chunk + MIN_CHUNK_SIZE;
                next->head = next_tag | next_size | PREV_INUSE_BIT;
                next->prev_foot = MIN_CHUNK_SIZE;
                insert_chunk(state, next, next_size);
                mte_color_tag(rem_chunk, remainder_n, tag_to_int(TAG_BITS));
            }else{
                return 0;
            }
            prev->prev_foot |= NEXT_PEN_BIT;
            return 0;
        }
    }

    int prev_op = remainder_p > 0? 1: prev != 0? 2: 0;

    if(remainder_n !=0 && remainder_n < MIN_CHUNK_SIZE && curr_inuse(next)){
        return 0;
    }else if(remainder_n > 0){
        if(remainder_n < MIN_CHUNK_SIZE){
            size_t next_size = chunk_size(next), next_tag = get_chunk_tag(next);
            unlink_chunk(state, next, next_size);
            next_size = next_size - (MIN_CHUNK_SIZE-remainder_n);
            remainder_n = MIN_CHUNK_SIZE;
            rem_chunk = align_as_chunk(unmap_end);
            rem_chunk->head = MIN_CHUNK_SIZE | TAG_BITS| INUSE_BITS| BLACKLIST_BIT;
            rem_chunk->prev_foot = unmap_size;
            next = rem_chunk + MIN_CHUNK_SIZE;
            next->head = next_tag | next_size | PREV_INUSE_BIT;
            next->prev_foot = MIN_CHUNK_SIZE;
            insert_chunk(state, next, next_size);
            mte_color_tag(rem_chunk, remainder_n, tag_to_int(TAG_BITS));
        }
        next->prev_foot = remainder_n;
        next = align_as_chunk(unmap_end);
        next->head = TAG_BITS| remainder_n| chunk->head & FLAG_BITS;
        next->prev_foot = PREV_EXH_BIT;
    }else if(next != 0){
        next->prev_foot = PREV_EXH_BIT;
    }

    switch (prev_op){
        case 1: {
            if(remainder_p < MIN_CHUNK_SIZE){
                rem_chunk = (char*)chunk-(MIN_CHUNK_SIZE-remainder_p);
                if(prev !=0){
                    unlink_chunk(state, prev, (chunk->prev_foot & ~EXHAUSTION_BITS));
                    prev->head = (prev->head & ~SIZE_BITS) | (char*)prev-(char*)rem_chunk;
                    insert_chunk(state, prev, (prev->head & SIZE_BITS));
                }
                rem_chunk->head = TAG_BITS|MIN_CHUNK_SIZE|CURR_INUSE_BIT;
                rem_chunk->prev_foot = prev ==0? NEXT_EXH_BIT|PREV_EXH_BIT: ((char*)prev-(char*)rem_chunk)|NEXT_EXH_BIT;
                mte_color_tag(rem_chunk, MIN_CHUNK_SIZE, tag_to_int(TAG_BITS));
            }else{
                prev = chunk;
                prev->head = remainder_p| TAG_BITS| chunk->head & FLAG_BITS;
                prev->prev_foot = chunk->prev_foot|NEXT_EXH_BIT;
            }
            break;
        }

        case 2: {
            prev->prev_foot |= NEXT_EXH_BIT;
            break;
        }
    }
    
    struct malloc_segment *sh = segment_holding(state, unmap_base);
    if(unmap_base == sh->base){
        sh->base = unmap_end;
        sh->size -= unmap_size;
        sh->blacklisted_size -= unmap_size;
    }
    return call_munmap(unmap_base, unmap_size);
}