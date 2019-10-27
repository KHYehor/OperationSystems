#include <stdio.h>
#include <stdlib.h>
#include "BuddyAllocatorList.h"

int main(int argc, const char * argv[]) {
    mem_init(1024, 64);
    void *p1 = mem_alloc(128);
    void *p2 = mem_alloc(64);
    void *p3 = mem_alloc(512);
    void *p4 = mem_alloc(128);
    void *p5 = mem_alloc(64);
    void *p6 = mem_alloc(64);
    mem_dump();
    mem_free(p2);
    mem_free(p5);
    mem_dump();
    mem_free(p4);
    mem_free(p6);
    mem_dump();
    return 0;
}
