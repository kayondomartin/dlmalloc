#ifndef TMTE_DLMALLOC_H
#define TMTE_DLMALLOC_H

#include <stdint.h>
#include <stddef.h>
#include <sys/mman.h>
#define SOFTBOUNDCETS_MMAP_FLAGS (MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE)
#define TAG_MASK ((size_t)-1U>>4)
#define TAG_BIT ~TAG_MASK
#define TAG_OFFSET TAG_MASK + 1U
#ifndef RISCV
extern char* __mte_tag_mem;
#endif

#ifdef RISCV
static inline int load_tag(void *addr) {
  int rv = 32;
  asm volatile ("ltag %0, 0(%1)"
                :"=r"(rv)
                :"r"(addr)
                );
  return rv;
}


static inline void store_tag(void *addr, int tag) {
  asm volatile ("stag %0, 0(%1)"
                :
                :"r"(tag), "r"(addr)
                );
}
#endif

static inline void mte_init(void){
  __mte_tag_mem = (char*) mmap(0, 0x0000100000000000 /* 8TB */, PROT_READ | PROT_WRITE, SOFTBOUNDCETS_MMAP_FLAGS, -1, 0);
}

static inline int mte_color_tag(char *base, long size, int tag_num) {
  long length  = (long)size / 2;//unit of size : byte, 4bit tag per 16 bit
#ifdef RISCV
  char *cur = (unsigned)base & 0xFFFFFFF0;
  if((int)base & 0x0F)
    length += 1;
  
  //tag_memset(cur,0,size);
  
#else
  char *tag_start = __mte_tag_mem + ((long)base >> 4);
  char *tag_end = __mte_tag_mem + ((long)(base + size - 1) >> 4);
  for (char *cur = tag_start; cur <= tag_end; cur++)
    *cur = tag_num;
#endif

  return tag_num;
}

static inline long mte_load_tag(char* base, long size){
#ifdef RISCV
  int base_tag = load_tag(base);
#else
  int base_tag = *(__mte_tag_mem + ((long)base >> 4));
#endif
  if (base_tag)
    return base_tag;
  else//need check
    return 0;
}

#endif