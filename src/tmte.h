#ifndef TMTE_DLMALLOC_H
#define TMTE_DLMALLOC_H

#include <stdint.h>
#include <stddef.h>
#define __USE_GNU
#include <sys/mman.h>
#undef __USE_GNU


#ifndef RISCV
#include "avl_tree.h"
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

static inline u_int8_t mte_color_tag(char *base, long size, u_int8_t tag_num) {
  long length  = (long)size / 2;//unit of size : byte, 4bit tag per 16 bit
#if defined( RISCV)
  char *cur = (unsigned)base & 0xFFFFFFF0;
  if((int)base & 0x0F)
    length += 1;
  
  //tag_memset(cur,0,size);
  
#else
 // if(size > 0x1000){
   // tag_num = avl_tree_insert(base, size, tag_num);
 // }else{
    char *tag_start = __mte_tag_mem + ((long)base >> 4);
    char *tag_end = __mte_tag_mem + ((long)(base + size - 1) >> 4);
    for (char *cur = tag_start; cur <= tag_end; cur++)
      *cur = tag_num;
  //}
#endif

  return tag_num;
}

static inline u_int8_t mte_load_tag(char* base, long size){
#ifdef RISCV
  int base_tag = load_tag(base);
#else
  size_t base_tag;
  if(size > 0x1000){
    base_tag = avl_tree_search(base);
  }else {
    base_tag = *(__mte_tag_mem + ((long)base >> 4));
  }
#endif
  if (base_tag)
    return base_tag;
  else//need check
    return -1;
}

#endif
