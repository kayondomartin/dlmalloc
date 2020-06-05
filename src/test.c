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
    dl_printf("Size of size_t=%ld\n", sizeof(size_t));
    dl_printf("\n-------------Test 1------------------- \n");
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
    dl_free(x); 
    p1 = dl_malloc(8);
    u_int64_t y_tag = get_chunk_tag(mem_to_chunk(y)); 
    dl_free(y); 
    p2 = dl_malloc(300);
    dl_assert((x == p1 && x_tag > p1_tag));
    dl_assert((y == p2 && y_tag > p2_tag));
    dl_printf("test 1: Chunk Reuse, distinct tags: PASSED\n");
    dl_free(p3);
    dl_printf("\n-------------Test 2----------------\n");
    p3 = dl_malloc(8);
    dl_free(p3);
    for(int i=0; i<16; ++i){
        x = dl_malloc(8);
        dl_free(x);
    }
    x = dl_malloc(8);
    size_t p3_tag = get_chunk_tag(mem_to_chunk(p3));
    x_tag = get_chunk_tag(mem_to_chunk(x));
    void *p4 = dl_malloc(300);
    dl_free(p4);
    for(int i=0; i<16; ++i){
        y = dl_malloc(300);
        dl_free(y);
    }
    size_t p4_tag = get_chunk_tag(mem_to_chunk(p4));
    y_tag = get_chunk_tag(mem_to_chunk(y));


    //Test 2
    dl_assert((x != p3 && x_tag < p3_tag && p3_tag == TAG_BITS));
    dl_assert((y != p4 && y_tag < p4_tag && p4_tag == TAG_BITS));
    dl_printf("test 2: Tag exhaustion, chunk Retirement: PASSED\n");
    dl_free(x);
    p3 = dl_malloc(8);
    p4 = dl_malloc(300);

    dl_printf("\n-------------Test 3 ----------------\n");
    void* p5;
    for(int i=0; i<3; ++i){
        p5 = dl_malloc(500);
        dl_free(p5);
    }
    p5 = dl_malloc(500);
    void* p6;
    for(int i=0; i<3; ++i){
        p6 = dl_malloc(500);
        dl_free(p6);
    }
    p6 = dl_malloc(500);
    dl_free(p5);
    size_t p5_tag = get_chunk_tag(mem_to_chunk(p5));

    x = dl_malloc(20);
    y = dl_malloc(300);

    x_tag = get_chunk_tag(mem_to_chunk(x));
    y_tag = get_chunk_tag(mem_to_chunk(y));

    dl_assert(((p5_tag == y_tag) && (x_tag == p5_tag)));
    dl_assert(p5 == x);
    dl_printf("test 3: Chunk reuse, bigger free chunk chop: PASSED\n");

    //Free all, start anew.
    dl_free(x);
    dl_free(y);
    dl_free(p1);
    dl_free(p2);
    dl_free(p3);
    dl_free(p4);
    dl_free(p6);
    dl_printf("\n-------------Test 4 ----------------\n");

    p1 = dl_malloc(60);
    p2 = dl_malloc(130);
    p3 = dl_malloc(450);

    p1_tag = get_chunk_tag(mem_to_chunk(p1));
    p2_tag = get_chunk_tag(mem_to_chunk(p2));
    p3_tag = get_chunk_tag(mem_to_chunk(p3));
    dl_assert((p1_tag != 0 && p2_tag != 0 && p3_tag !=0));
    dl_printf("test 4: Arbitrary chunk alloc, check tag coloring: PASSED\n");
    p4 = dl_malloc(2064);
    for(int i=0; i<26; i++){
        dl_free(p4);
        dl_printf("here: i=%d p4_tag=0x%016lX\n",i,get_chunk_tag(mem_to_chunk(p4)));
        p4 = dl_malloc(2064);
    }
    
    dl_free(p4);
    int size = 5400;
    p5 = dl_malloc(size);
    for(int i=0; i<15; i++){
        dl_free(p5);
        p5 = dl_malloc(size);
    }
    dl_free(p5);

    p6 = dl_malloc(200);
    dl_free(p6);
    dl_printf("\ninspect all\n");
    dl_malloc_inspect_all(&inspector, 0);
    dl_printf("\n\n----------End of Tests-----------\n\n");
}

int main() {
    test_dl();

    return 0;
}
