#ifndef TMTE_DLMALLOC_H
#define TMTE_DLMALLOC_H

#include <stdint.h>
#include <stddef.h>
#ifdef AARCH64
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/auxv.h>
#endif
#define __USE_GNU
#include <sys/mman.h>
#ifdef AARCH64
#include <sys/prctl.h>
#endif
#define SOFTBOUNDCETS_MMAP_FLAGS (MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE)
#undef __USE_GNU

#if DECOMPOSE_OVERHEAD
#include <sys/time.h>
#endif

#ifndef RISCV
extern char* __mte_tag_mem;
#ifdef AARCH64
#define HWCAP2_MTE              (1 << 18)
#define PROT_MTE                (0x20)
#define PR_SET_TAGGED_ADDR_CTRL 55
#define PR_GET_TAGGED_ADDR_CTRL 56
#define PR_TAGGED_ADDR_ENABLE   (1UL << 0)
#define PR_MTE_TCF_SHIFT        1
#define PR_MTE_TCF_NONE         (0UL << PR_MTE_TCF_SHIFT)
#define PR_MTE_TCF_SYNC         (1UL << PR_MTE_TCF_SHIFT)
#define PR_MTE_TCF_ASYNC        (2UL << PR_MTE_TCF_SHIFT)
#define PR_MTE_TCF_MASK         (3UL << PR_MTE_TCF_SHIFT)
#define PR_MTE_TAG_SHIFT        3
#define PR_MTE_TAG_MASK         (0xffffUL << PR_MTE_TAG_SHIFT)
#endif
#endif

#if DECOMPOSE_OVERHEAD
double elapsed_search;
double elapsed_write;
double elapsed_update;
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

#ifdef AARCH64
/*
* Insert a random logical tag into the given pointer.
*/
#define insert_random_tag(ptr) ({                       \
       uint64_t __val;                                 \
       asm("irg %0, %1" : "=r" (__val) : "r" (ptr));   \
        __val;                                          \
})

/*
* Set the allocation tag on the destination address.
*/
#define set_tag(tagged_addr) do {                                      \
       asm volatile("stg %0, [%0]" : : "r" (tagged_addr) : "memory"); \
} while (0)
#endif

static inline void mte_init(void){
#ifndef TEST_MEMORY
  #ifdef AARCH64
    unsigned long hwcap2 = getauxval(AT_HWCAP2);

    /* check for availability of MTE */
    if(!(hwcap2 & HWCAP2_MTE)){
      abort();
    }

    /* Enable the tagged address ABI, synchronous MTE tag check faults and
     * allow all non-zero tags in the randomly generated set.
     */
    if (prctl(PR_SET_TAGGED_ADDR_CTRL,
          PR_TAGGED_ADDR_ENABLE | PR_MTE_TCF_SYNC | (0xfffe << PR_MTE_TAG_SHIFT),
             0, 0, 0)) {
            perror("prctl() failed");
            abort();
    }

    /* init tag mem */
    __mte_tag_mem = (char*) mmap(0, 0x0000100000000000, PROT_MTE| PROT_READ|PROT_WRITE, SOFTBOUNDCETS_MMAP_FLAGS, -1, 0);
    
    if(__mte_tag_mem == MAP_FAILED){
      abort();
    }

  #else
    __mte_tag_mem = (char*) mmap(0, 0x0000100000000000 /* 8TB */, PROT_READ | PROT_WRITE , SOFTBOUNDCETS_MMAP_FLAGS, -1, 0);
  #endif
#endif
}

static inline u_int8_t mte_color_tag2(char *base, long size, u_int8_t tag_num) {
#ifndef TEST_MEMORY
  char *tag_start = __mte_tag_mem + ((long)base >> 4);
  char *tag_end = __mte_tag_mem + ((long)(base + size - 1) >> 4);
  for (char *cur = tag_start; cur <= tag_end; cur++)
    *cur = tag_num;
#endif

  return tag_num;

}
static long total2 = 0;
static long total3 = 0;
static long hundred_mega = 1000000000;
static inline u_int8_t mte_color_tag(char *base, long size, u_int8_t tag_num) {
#if DECOMPOSE_OVERHEAD
  struct timeval begin,end;
  long seconds;
  long microseconds;
  gettimeofday(&begin, 0);
#endif

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
#ifndef TEST_MEMORY
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
#endif
#if DECOMPOSE_OVERHEAD
          gettimeofday(&end, 0);
          seconds = end.tv_sec - begin.tv_sec;
          microseconds = end.tv_usec - begin.tv_usec;
          elapsed_write = seconds + microseconds*1e-6;
          
          dl_printf("elapsed_write : %.3f sec.\n",elapsed_write);
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
