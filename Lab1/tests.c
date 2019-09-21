#include "tests.h"
#include "allocator.h"
#include <assert.h>
#include <stdbool.h>

static void mem_alloc_init_test()
{
    printf("Mem_alloc_init testing...\n");
    bool first_init = mem_alloc_init(1000);
    bool second_init = mem_alloc_init(1000);
    assert(first_init != second_init);
    printf("Mem_alloc_init passed!\n");
    mem_alloc_free();
};

static void mem_alloc_test()
{
    printf("Mem_alloc testing...\n");
    assert(mem_alloc(100) == NULL);
    printf("alloc without init passed!\n");
    
    mem_alloc_init(100);
    assert(mem_alloc(200) == NULL);
    printf("Not enough space passed!\n");
    mem_alloc_free();
    
    mem_alloc_init(400);
    assert(mem_alloc(100) != NULL);
    assert(mem_alloc(100) != NULL);
    assert(mem_alloc(100) != NULL);
    assert(mem_alloc(100) == NULL);
    printf("Successful creation!\n");
    mem_alloc_free();
};

static void mem_free_test()
{
    mem_alloc_init(100000);
    mem_alloc(10000); mem_alloc(5000);
    void *a1 = mem_alloc(5000);
    void *a2 = mem_alloc(10000);
    void *a3 = mem_alloc(20000);
    void *a4 = mem_alloc(30000);
    mem_alloc(10000); mem_alloc(5000);
    mem_alloc(10000); mem_alloc(5000);
    mem_free(a2); mem_free(a3); mem_free(a4); mem_free(a1);
    void *a5 = mem_alloc(40000);
    assert(a1 == a5);
    printf("Memory free Successful!\n");
    mem_alloc_free();
};

static void mem_realloc_test()
{
    mem_alloc_init(1000);
    void *old = mem_alloc(500);
    void *new = mem_realloc(old, 700);
    assert(new != NULL);
    assert(mem_realloc(new, 1000) == NULL);
    printf("Successful Realloc!\n");
};

void mem_run_tests()
{
    mem_alloc_init_test();
    mem_alloc_test();
    mem_free_test();
    mem_realloc_test();
};
