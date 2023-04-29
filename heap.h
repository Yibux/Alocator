//
// Created by root on 12/4/22.
//

#ifndef ALOKATORSO2_HEAP_H
#define ALOKATORSO2_HEAP_H
#include <stdio.h>

enum status {
    OCCUPIED = 1,
    LAST = 2
};

struct memory_manager_t
{
    void *memory_start;
    int hashSum;
    struct memory_chunk_t *first_memory_chunk;
    enum status mStatus;

};

struct memory_chunk_t
{
    struct memory_chunk_t* prev;
    struct memory_chunk_t* next;
    size_t size;
    int free;
    int hash;
};

enum pointer_type_t
{
    pointer_null,
    pointer_heap_corrupted,
    pointer_control_block,
    pointer_inside_fences,
    pointer_inside_data_block,
    pointer_unallocated,
    pointer_valid
};


int getHash(struct memory_chunk_t *pom);
int heap_validate(void);
int heap_setup(void);
void heap_clean(void);

void* heap_calloc(size_t number, size_t size);
void* heap_realloc(void* memblock, size_t count);
void  heap_free(void* memblock);
void* heap_malloc(size_t size);
int free_number(struct memory_chunk_t *chunk, int *size);
int hashSum(void);

size_t heap_get_largest_used_block_size(void);
enum pointer_type_t get_pointer_type(const void* const pointer);

#endif //ALOKATORSO2_HEAP_H
