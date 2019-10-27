#ifndef list_h
#define list_h

#include <stdio.h>
#include <stdbool.h>

void mem_init(size_t size, size_t min_size);
// Выделение памяти
void* mem_alloc(size_t size);
// Логгер
void mem_dump(void);
// Освобождение памяти
void* mem_free(void* addr);
#endif

