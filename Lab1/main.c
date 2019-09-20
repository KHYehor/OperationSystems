#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"

int main()
{
    mem_alloc_init(4000);
    void *a = mem_alloc(100);
    void *b = mem_alloc(100);
    void *c = mem_alloc(100);
    void *d = mem_alloc(100);
    void *e = mem_alloc(100);
    void *f = mem_alloc(100);
    void *g = mem_alloc(100);
    void *h = mem_alloc(100);
    printf("New Adress: %p \n", a);
    printf("New Adress: %p \n", b);
    mem_free(b);
    printf("New Adress: %p \n", c);
    mem_free(c);
    printf("New Adress: %p \n", d);
    mem_free(d);
    mem_free(a);
    printf("New Adress: %p \n", e);
    printf("New Adress: %p \n", f);
    printf("New Adress: %p \n", g);
    printf("New Adress: %p \n", h);
    mem_dump();
    return 0;
}
