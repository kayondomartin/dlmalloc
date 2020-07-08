#ifndef TMTE_DLMALLOC_H
#define TMTE_DLMALLOC_H

#include <stdint.h>
#include <stddef.h>
#define __USE_GNU
#include <sys/mman.h>
#define SOFTBOUNDCETS_MMAP_FLAGS (MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE)
#undef __USE_GNU


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
  //init_avl_tree();
}

static inline u_int8_t mte_color_tag2(char *base, long size, u_int8_t tag_num) {
  char *tag_start = __mte_tag_mem + ((long)base >> 4);
  char *tag_end = __mte_tag_mem + ((long)(base + size - 1) >> 4);
  for (char *cur = tag_start; cur <= tag_end; cur++)
    *cur = tag_num;

  return tag_num;

}
static long total2 = 0;
static long total3 = 0;
static long hundred_mega = 1000000000;
static inline u_int8_t mte_color_tag(char *base, long size, u_int8_t tag_num) {
/*  total2 += size;
  if(total2/hundred_mega != total3/hundred_mega){
    total3 = total2;
    dl_printf("iyb: total %lu\n", total2);
  }
  if(size>0x1000000)
    {
      dl_printf("");
    }

  return tag_num;*/
#if defined( RISCV)
  long length  = (long)size / 2;//unit of size : byte, 4bit tag per 16 byte
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

static inline u_int8_t mte_load_tag(char* base, long size){
#ifdef RISCV
  int base_tag = load_tag(base);
#else
  int base_tag = *(__mte_tag_mem + ((long)base >> 4));
#endif
  if (base_tag)
    return base_tag;
  else//need check
    return -1;
}

#endif
