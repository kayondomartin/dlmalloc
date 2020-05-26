#include <stdint.h>

#include "malloc.h"
#include "log.h"
#include "chunk.h"
#include "assert.h"

void inspector(void *start, void *end, size_t used_bytes, void *callback_arg) {
    (void) callback_arg; // unused
    dl_printf(
            "start=0x%016lX end=0x%016lX size=%6lu used_bytes=%6lu\n",
            (uintptr_t) start, (uintptr_t) end, (end - start), used_bytes);
}

void test_dl() {
    dl_printf("\n--------------Test 1------------------- \n");
    dl_printf("TAG_BITS=0x%016lX\n", (uintptr_t) TAG_BITS);
    dl_printf("TAG_OFFSET=0x%016lX\n", (uintptr_t) TAG_OFFSET);
    dl_printf("TAG_MASK=0x%016lX\n", (uintptr_t) TAG_MASK);
    void *p1 = dl_malloc(8);
    struct malloc_chunk* p1_chunk = mem_to_chunk(p1);
    size_t p1_tag = get_chunk_tag(p1_chunk);
    void *p2 = dl_malloc(300);
    void *p3 = dl_malloc(1024 * 1024);

    dl_free(p1);
    dl_free(p2);

#ifdef DEBUG
    dl_printf("\nprint allocations\n");
    dl_print_allocations();
    dl_printf("------\n");
#endif

    void *x = dl_malloc(8);
    //Test 1
    size_t x_tag = get_chunk_tag(p1_chunk);
    dl_assert((x == p1 && x_tag > p1_tag));
    dl_printf("test 1: Chunk Reuse, distinct tags: PASSED\n");
    struct malloc_chunk* x_c = mem_to_chunk(x);
    dl_free(x);

    dl_free(p3);

    dl_printf("\n------------Test 2----------------\n");
    for(int i=0; i<15; ++i){
        x = dl_malloc(8);
        struct malloc_chunk* x_c = mem_to_chunk(x);
        dl_free(x);
    }
    //Test 2
    size_t new_chunk_tag = get_chunk_tag(x_c);
    x_tag = get_chunk_tag(p1_chunk);
    dl_assert((x != p1 && new_chunk_tag < x_tag && x_tag == TAG_BITS))
    dl_printf("test 2: Tag exhaustion, chunk Retirement: PASSED\n");


    dl_printf("\ninspect all\n");
    dl_malloc_inspect_all(&inspector, 0);
    dl_printf("\n\n----------End of Tests-----------\n\n");
}

int main() {
    test_dl();

    return 0;
}
