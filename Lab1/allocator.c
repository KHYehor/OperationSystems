#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Заготавливаем область памяти
static Memory_Pool pool = { 0, NULL, NULL };
static Memory_Pool *mpool = &pool;

bool mem_alloc_init(size_t size)
{
    // Если аллокатор уже сделан, то второй раз делать его не будем
    if (mpool->size != 0) return false;
    // Создаем начальный хедер
    Header header = { size - sizeof(Header), false, true };
    Header *first_header = (Header *)(malloc(size));
    *first_header = header;
    // Указываем в аллокаторе его первый хедер, размер и конечную границу памяти
    mpool->start = (void *)first_header;
    mpool->size = size;
    mpool->end = mpool->start + size;
    // Логирование
    printf("Start adress: %p \n", mpool->start);
    printf("End adress: %p \n", mpool->end);
    printf("Size %ld \n", mpool->end - mpool->start);
    // Сообщаем что создание прошло успешно
    return true;
};

static void mem_dump_helper(void *current, int number)
{
    Header *header = (Header *)current;
    printf("----------------------------------------\n");
    printf("Number: %d\n", number);
    printf("Block Size: %zu\n", header->curr_size);
    printf("Free: %d\n", header->free);
    if (header->next_exists) return mem_dump_helper(current + sizeof(Header) + header->curr_size, ++number);
    printf("========================================\n");
    return;
};

void mem_dump()
{
    printf("========================================\n");
    printf("Memory start: %p\n", mpool->start);
    printf("Memory pool size: %ld\n", mpool->size);
    printf("Memory end: %p\n", mpool->end);
    mem_dump_helper(mpool->start, 0);
};

static void *mem_alloc_helper(void *current, void *end, size_t size, void *best_address, size_t best_size)
{
    Header *address = (Header *)current;
    // Если находим такой блок памяти, с ровно нужной нам областью памяти, то пишем в него
    if (address->free && address->curr_size == size)
    {
        address->curr_size = size; address->free = false;
        return (void *)address + sizeof(Header);
    }
    // Если находим свободный блок, с более чем нужной нам памятью и эта память
    // еще меньше чем та, которую мы нашли ранее то запоминаем ее
    else if (address->free && size < address->curr_size && (address->curr_size < best_size))
    {
        best_address = current; best_size = ((Header *)current)->curr_size;
    }
    // Смотрим можем ли мы идти дальше
    if (address->next_exists) return mem_alloc_helper((current + sizeof(Header) + address->curr_size), end, size, best_address, best_size);
    // Смотрим, находили ли мы подходящий блок ранее
    else if (best_address != NULL)
    {
        Header *best_header = (Header *)best_address;
        // Проверяем, можем ли мы пилить
        if (best_header->curr_size - size - sizeof(Header) > 8)
        {
            // Создаем новый хедр
            Header new_header = { best_header->curr_size - size - sizeof(Header), best_header->next_exists, true };
            Header *next_header = (Header *)(best_address + sizeof(Header) + size);
            *next_header = new_header;
            best_header->curr_size = size;
            best_header->next_exists = true;
            best_header->free = false;
            return (best_address + sizeof(Header));
        }
        // Пилить в пыль нет смысла, поэтому возвращаем то, что есть
        return NULL;
    }
    // Смотрим, можем ли мы создать новый блок с необходимым размером дальше
    else if ((current + (2 * sizeof(Header)) + address->curr_size + size) < end)
    {
        // Говорим текущему хедеру, что дальше будет хедер
        address->next_exists = true;
        Header new_header = { size, false, false };
        Header *next_header = (Header *)(current + sizeof(Header) + address->curr_size);
        *next_header = new_header;
        // Возвращаем указатель на выделенную область памяти
        return ((void *)next_header + sizeof(Header));
    }
    // Если у нас нет лучшего адреса, и мы не можем выделить место под новый блок, тогда места нет вообще
    else return NULL;
};

void *mem_alloc(size_t size)
{
    if (mpool->size <= 0 || size <= 0) return NULL;
    return (void *)mem_alloc_helper(mpool->start, mpool->end, size, NULL, mpool->size);
};

void *mem_realloc(void *addr, size_t size)
{
    mem_free(addr);
    void *new_addr = mem_alloc(size);
    if (new_addr == NULL) return NULL;
    memcpy(new_addr, addr, ((Header *)(addr - sizeof(Header)))->curr_size);
    return new_addr;
};

static void* mem_free_helper(void *current)
{
    Header *address = (Header *)current;
    Header *next_adress = (Header *)(current + sizeof(Header) + address->curr_size);
    if (address->next_exists && next_adress->free) return mem_free_helper((void *)next_adress);
    return (void *)next_adress;
}

bool mem_free(void *addr)
{
    Header *address = (Header *)(addr - sizeof(Header));
    Header *next_adress = (Header *)(addr + address->curr_size);
    if (address->next_exists && next_adress->free) address->curr_size = mem_free_helper(addr - sizeof(Header)) - addr;
    address->free = true;
    return true;
};

bool mem_alloc_free()
{
    if (mpool->size == 0) return false;
    mpool->size = 0; mpool->end = mpool->start;
    return true;
};
