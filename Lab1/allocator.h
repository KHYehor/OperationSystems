#ifndef allocator_h
#define allocator_h

#include <stdbool.h>
#include <stdio.h>

typedef struct Header
{
    size_t curr_size;
    bool next_exists;
    bool free;
} Header;

typedef struct MPool
{
    size_t size;
    void *start;
    void *end;
} Memory_Pool;

bool mem_alloc_init(size_t size);

void mem_dump(void);

void *mem_alloc(size_t size);

void *mem_realloc(void *addr, size_t size);

bool mem_free(void *addr);

#endif
