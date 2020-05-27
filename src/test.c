#include <stdint.h>

#include "malloc.h"
#include "log.h"
#include "chunk.h"

#define dl_assert(x)\
{\
    if(!(x))\
        abort();\
} 

void inspector(void *start, void *end, size_t used_bytes, void *callback_arg) {
    (void) callback_arg; // unused
    dl_printf(
            "start=0x%016lX end=0x%016lX size=%6lu used_bytes=%6lu\n",
            (uintptr_t) start, (uintptr_t) end, (end - start), used_bytes);
}

void test_dl() {
    dl_printf("\n---------Running tests: Including TMTE--------------\n\n");
    dl_printf("TAG_BITS=0x%016lX\n", (uintptr_t) TAG_BITS);
    dl_printf("TAG_OFFSET=0x%016lX\n", (uintptr_t) TAG_OFFSET);
    dl_printf("TAG_MASK=0x%016lX\n", (uintptr_t) TAG_MASK);
    dl_printf("\n--------------Test 1------------------- \n");
    void *p1 = dl_malloc(8);
    u_int64_t p1_tag = get_chunk_tag(mem_to_chunk(p1));
    void *p2 = dl_malloc(300);
    u_int64_t p2_tag = get_chunk_tag(mem_to_chunk(p2));
    void *p3 = dl_malloc(1024 * 1024);

    dl_free(p1);
    dl_free(p2);

    void *x = dl_malloc(8);
    void *y = dl_malloc(300);
    //Test 1
    u_int64_t x_tag = get_chunk_tag(mem_to_chunk(x));
    u_int64_t y_tag = get_chunk_tag(mem_to_chunk(y));
    dl_assert((x == p1 && x_tag > p1_tag));
    dl_assert((y == p2 && y_tag > p2_tag));
    dl_printf("test 1: Chunk Reuse, distinct tags: PASSED\n");
    dl_free(x);
    dl_free(y);

    dl_free(p3);

    dl_printf("\n------------Test 2----------------\n");
    for(int i=0; i<20; ++i){
        x = dl_malloc(8);
        dl_free(x);
    }
    x_tag = get_chunk_tag(mem_to_chunk(x));
    p1_tag = get_chunk_tag(mem_to_chunk(p1));

    for(int i=0; i<15; ++i){
        y = dl_malloc(300);
        dl_free(y);
    }
    y_tag = get_chunk_tag(mem_to_chunk(y));
    p2_tag = get_chunk_tag(mem_to_chunk(p2));

    //Test 2
    dl_assert((x != p1 && x_tag < p1_tag && p1_tag == TAG_BITS));
    dl_assert((y != p2 && y_tag < p2_tag && p2_tag == TAG_BITS));
    dl_printf("test 2: Tag exhaustion, chunk Retirement: PASSED\n");

    dl_printf("\n-------------Test 3 ----------------\n");
    p2 = dl_malloc(257);
    y = dl_malloc(20);
    p2_tag = get_chunk_tag(mem_to_chunk(p2));
    y_tag = get_chunk_tag(mem_to_chunk(y));
    p1 = dl_malloc(100);
    dl_free(p1);
    x = dl_malloc(50);
    p1 = dl_malloc(20);
    p1_tag = get_chunk_tag(mem_to_chunk(p1));
    x_tag = get_chunk_tag(mem_to_chunk(x));
    dl_free(p1);
    dl_free(x);
    dl_free(y);
    dl_free(p2);

    dl_printf("long size: %d", sizeof(long));
    //Test 3
    dl_assert(p2_tag == y_tag);
    dl_assert(p1_tag == x_tag);
    dl_printf("test 3: Chunk reuse, bigger free chunk chop: PASSED\n");

    dl_printf("\ninspect all\n");
    dl_malloc_inspect_all(&inspector, 0);
    dl_printf("\n\n----------End of Tests-----------\n\n");
}

int main() {
    test_dl();

    return 0;
}
