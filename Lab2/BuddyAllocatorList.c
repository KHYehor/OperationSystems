#include "BuddyAllocatorList.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// Структура списка
typedef struct BuddyList
{
    size_t size_power;
    size_t size;
    size_t MIN_SIZE;
    void *first_node;
} List;

// Структура узла
typedef struct BuddyNode
{
    // Степень текущей памяти
    unsigned short int power;
    // Индекс текущего списка
    unsigned short int index;
    // Статус текущей памяти
    bool free;
    // Адресс на следующую память
    void *next;
    // Адресс на выделяемую память
    void *addr;
} Node;

// Загатавливаю список
static List BuddyList = { 0, 0, 0, NULL };
static List *Blist = &BuddyList;

// Инициализируем список
void mem_init(size_t size, size_t min_size)
{
    // Создаю первый узел
    Node BuddyNode = { 0, 0, false, NULL, NULL };
    Node *FirstNode = (Node *)malloc(sizeof(Node));
    *FirstNode = BuddyNode;
    // Заполняем первый элемент списка
    FirstNode->free = true;
    FirstNode->power = log(size / min_size)/log(2);
    FirstNode->index = 0;
    FirstNode->next = NULL;
    FirstNode->addr = malloc(size);
    // Заполняем список
    Blist->size = size;
    Blist->MIN_SIZE = min_size;
    Blist->size_power = log(size) / log(2);
    Blist->first_node = (void *)FirstNode;
    printf("Memory pool inited!\n");
}

// Поиск нужного узла
Node* list_find_index(Node* node, int index)
{
    // Найден нужный узел
    if (node->index == index) return node;
    // Дальше есть узел, ищем там
    if (node->next != NULL) return list_find_index((Node *)node->next, index);
    return NULL;
}

// Инкрементация индексов списка
void list_increment(Node* node)
{
    node->index = node->index + 1;
    if (node->next) list_increment((Node *)(node->next));
}

// Декрементация индексов списка
void list_decrement(Node* node)
{
    node->index = node->index - 1;
    if (node->next != NULL) list_increment((Node *)(node->next));
}

// Вставка нового узла, после конретного
void list_insert_after(int index)
{
    // Поиск узла
    Node* found_node = list_find_index(Blist->first_node, index);
    // Создаем новый узел
    Node new_node = {
        found_node->power - 1,
        found_node->index + 1,
        true,
        found_node->next,
        found_node->addr + (Blist->MIN_SIZE * (size_t)pow(2, found_node->power - 1))
    };
    Node* new_nodep = (Node *)malloc(sizeof(Node));
    *new_nodep = new_node;
    // Модифицируем старый
    found_node->power = (int)(found_node->power - 1);
    found_node->next = (void *)new_nodep;
    // Инкрементировать остальные индексы
    if (new_nodep->next) list_increment((Node *)(new_nodep->next));
}

// Объеденение двух узлов
void list_delete_after(int index)
{
    // Поиск узла
    Node* found_node = list_find_index(Blist->first_node, index);
    Node* delete_node = (Node *)found_node->next;
    // Модифицируем старый
    found_node->power = found_node->power + 1;
    found_node->next = delete_node->next;
    // Декрементируем остальные индексы
    list_decrement((Node *)(found_node->next));
    // Удаляем старый узел из памяти
    free((void *)delete_node);
}

// Функция для разбивания узла до нужного размера
void* devide_node(Node* node, size_t from, size_t to)
{
    // Разбиение закончено
    if (from == to)
    {
        node->free = false;
        return node->addr;
    }
    // Продолжить разбиение
    list_insert_after(node->index);
    return devide_node(node, from - 1, to);
}

// Вспомогательная функция для поиска узла конкретного размера
Node* list_find_space_recursion(Node* node, size_t power)
{
    if (node->free && node->power == power) return node;
    if (node->next) return list_find_space_recursion((Node *)node->next, power);
    return NULL;
}

// Вспомогательная функция для поиска узла с конкретным адрессом
Node* list_find_space_helper(Node* node, size_t current_power)
{
    Node* found_node = list_find_space_recursion(node, current_power);
    if (found_node != NULL) return found_node;
    if (current_power + 1 <= Blist->size_power) return list_find_space_helper(node, current_power + 1);
    return NULL;
}

// Находим максимально удобное место для списка
void* mem_alloc(size_t size)
{
    // Определяем необходимую степень
    size_t find_power = log(size / Blist->MIN_SIZE) / log(2);
    // Ищем подходящий узел
    Node* found_node = list_find_space_helper((Node *)(Blist->first_node), find_power);
    if (!found_node) return NULL;
    // Если размеры узлов совпадают, то отдаем его
    if (found_node->power == find_power)
    {
        found_node->free = false;
        return found_node->addr;
    }
    // Делим узел до необходимого нам размера
    return devide_node(found_node, found_node->power, find_power);
}

// Оптимизированное освобождение памяти
void optimize_space(Node *node)
{
    Node* next_node = (Node *)node->next;
    // Дальше узла нет, оптимизация закончена
    if (!next_node) return;
    // Если степени не равны, то идеи дальше
    if (node->power != next_node->power) return optimize_space(next_node);
    // Два рядом стоящих элемента можно соеденить
    if (node->free && next_node->free && node->power == next_node->power)
    {
        // Изменяем данные первого
        node->next = next_node->next;
        node->power = node->power + 1;
        // Осовбождаем память
        free(next_node);
        // Изменяем индексы последющих узлов
        list_decrement((Node *)node->next);
        // Продолжаем оптимизацию
        return optimize_space(Blist->first_node);
    }
    // Два рядом стоящих с подходящей степенью, но один из них занят,
    // смотрим можем ли мы перешагнуть через этот узел
    if (node->power == next_node->power && (node->free || next_node->free) && ((Node *)(next_node->next))->next)
        return optimize_space((Node *)next_node->next);
    // Если перешгануть не вышло, оптимизация закончена
    if (!((Node *)(next_node->next))->next) return;
}

// Найти необходимый узел, по адресу, что он предоставляет
Node* list_find_node(Node* node, void* addr)
{
    if (node->addr == addr) return node;
    if (node->next) return list_find_node((Node *)node->next, addr);
    return NULL;
}

// Освобождение памяти
void* mem_free(void* addr)
{
    Node* node = list_find_node((Node *)Blist->first_node, addr);
    // Если такого адрееса не найдено, возвращаем нулл, безуспешное освобождение
    if (node) node->free = true;
    else return NULL;
    optimize_space((Node *)Blist->first_node);
    // Если освобождение успешное, тогда возвращаем этот же адресс
    return addr;
}

// Рекурсивный вывод каждого узла
void mem_dump_helper(Node* node)
{
    printf("-------------------------------\n");
    printf("Node Index: %hu\n", node->index);
    printf("Node Size: %f\n", pow(2, node->power) * Blist->MIN_SIZE);
    printf((node->free ? "Node Free: true\n" : "Node Free: false\n"));
    printf("-------------------------------\n");
    if (node->next) mem_dump_helper((Node *)node->next);
}

// Вывод аллокатора
void mem_dump()
{
    printf("===============================\n");
    printf("Buddy Memory Allocator\n");
    printf("Memory Size(bytes): %zu\n", Blist->size);
    printf("===============================\n");
    mem_dump_helper((Node *)Blist->first_node);
    printf("===============================\n");
}
