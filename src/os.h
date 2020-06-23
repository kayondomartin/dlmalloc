#ifndef MALLOC_ALLOC_H
#define MALLOC_ALLOC_H

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define __USE_GNU 1

#include <sys/mman.h>

#undef __USE_GNU

#include "config.h"
#include "sbrk.h"
#include "redblack.h"

/* MORECORE and MMAP must return MFAIL on failure */
#define MFAIL                   ((void*) -1)

#if DBG
extern size_t brk_addr;
#endif
size_t watermark;

static inline void *call_sbrk(intptr_t increment) {
#if defined(DISABLE_SBRK)
  (void) increment; // unused
  return MFAIL;
#elif defined(EMULATE_SBRK)
  return emulate_sbrk(increment);
#elif !defined(__APPLE__)//iyb: used
#if DBG
  size_t addr;
  if(!increment) increment = 3*UNMAP_UNIT;
  if (!watermark){
    addr = sbrk(increment);
    watermark = addr+increment;
  }
  else{
    addr = watermark;
    int res = mmap(watermark, increment, PROT_READ | PROT_WRITE | MAP_FIXED, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    watermark += increment;
  }
//    size_t addr = sbrk(increment);
  if(brk_addr == 0 )
    brk_addr = addr;
  dl_printf("iyb: sbrk program break extended by 0x%llx.\n", addr-brk_addr);
return addr;
#else
return sbrk(increment);
#endif//DBG

#else
(void) increment; // unused
return MFAIL;
#endif//DISABLE_SBRK
}

static inline void *call_mmap(size_t size) {
    return mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

static inline int call_munmap(void *p, size_t size) {
    return munmap(p, size);
}

static inline void *call_mremap(void *old_address, size_t old_size, size_t new_size, int flags) {
#if !defined(__APPLE__)
    return mremap(old_address, old_size, new_size, flags);
#else
    (void) old_address; // unused
    (void) old_size; // unused
    (void) new_size; // unused
    (void) flags; // unused
    return MFAIL;
#endif
}

struct malloc_state;
struct malloc_chunk;

void *mmap_alloc(struct malloc_state *state, size_t size);

struct malloc_chunk *mmap_resize(struct malloc_state *state, struct malloc_chunk *old_p, size_t size, int flags);

void *sys_alloc(struct malloc_state *state, size_t size);

int sys_trim(struct malloc_state *state, size_t pad);

size_t release_unused_segments(struct malloc_state *state);

/* tmte edit: ops */
void release_exhausted_segment(struct malloc_state *state, struct malloc_segment* segment);

void replace_released_segment(struct malloc_state *state, struct malloc_segment* pseg, struct malloc_segment* nseg);

void release_exhausted_chunk(struct malloc_state *state, struct malloc_segment* segment, struct malloc_chunk* prev, char* base, size_t size);

#endif //MALLOC_ALLOC_H
