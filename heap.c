//
// Created by root on 12/4/22.
//

#include "heap.h"
#include "tested_declarations.h"
#include "rdebug.h"
#include "tested_declarations.h"
#include "rdebug.h"
#include "tested_declarations.h"
#include "rdebug.h"
#include "tested_declarations.h"
#include "rdebug.h"
struct memory_manager_t memory_manager;
int allsizemalloc = 0;
int allsizefree = 0;


int heap_setup(void)
{
    memory_manager.memory_start = custom_sbrk(0);
    if(memory_manager.memory_start == (void*)-1)
        return 1;
    memory_manager.mStatus = LAST;
    memory_manager.first_memory_chunk = 0;
    return 0;
}
void heap_clean(void)
{
    size_t structSize = sizeof(struct memory_chunk_t);
    struct memory_chunk_t *first = (struct memory_chunk_t *)memory_manager.first_memory_chunk;
    size_t heapCapacity = 0;
    for (; first ; first=first->next) {
        if(first->next && (first->next->free == 0 || first->next->free))
        {
            heapCapacity += (size_t)((char*)first->next - (char*)first);
        }
        else
        {
            heapCapacity += structSize + 4 + first->size;
        }
    }
    memory_manager.hashSum = 0;
    memory_manager.memory_start = custom_sbrk(-1*(intptr_t)heapCapacity);
    printf("\n\nheap capacity: %d\n\n",(int)heapCapacity);
    memory_manager.memory_start = NULL;
    memory_manager.first_memory_chunk = NULL;

}
void* heap_malloc(size_t size)
{
    size_t structSize = sizeof(struct memory_chunk_t);
    if(!size || heap_validate())
        return NULL;

    if(!memory_manager.first_memory_chunk)
    {
        memory_manager.memory_start = custom_sbrk(size+4+structSize);
        if(memory_manager.memory_start == (void*)-1)
            return NULL;
        struct memory_chunk_t *first = (struct memory_chunk_t *)memory_manager.memory_start;
        first->next = NULL;
        first->prev = NULL;
        first->free = 0;
        first->size = size;
        allsizemalloc += size + 4 + structSize;

        for (int i = 0; i < 2; ++i) {
            *((char*)first+i+structSize) = '#';
            *((char*)first+i+size+structSize+2) = '#';
        }

        memory_manager.first_memory_chunk = first;
        memory_manager.first_memory_chunk->hash = getHash(memory_manager.first_memory_chunk);
        memory_manager.hashSum = hashSum();
        return (char *)memory_manager.first_memory_chunk + structSize + 2;
    }
    else
    {
        struct memory_chunk_t *prevPom = NULL;
        struct memory_chunk_t *pom = memory_manager.first_memory_chunk;
        for(; pom ; prevPom = pom, pom=pom->next)
        {
            if((pom->size >= size + 4 && pom->free))
                break;
        }
        if(pom)
        {
            for (int i = 0; i < 2; ++i) {
                *((char*)pom+i+structSize) = '#';
            }
            for (int i = 0; i < 2; ++i) {
                *((char*)pom+i+size+structSize+2) = '#';
            }
            pom->free = 0;
            pom->size = size;
            allsizemalloc += size + 4;
            pom->hash = getHash(pom);
            memory_manager.hashSum = hashSum();
            return (char *)pom + structSize + 2;
        }
        pom = custom_sbrk(size+4+structSize);
        if(pom == (void*)-1)
        {
            pom = NULL;
            return NULL;
        }
        allsizemalloc += size + 4 + structSize;

        prevPom->next = pom;

        pom->next = NULL;
        pom->prev = prevPom;
        pom->size = size;
        pom->free = 0;
        pom->hash = getHash(pom);
        prevPom->hash = getHash(prevPom);
        for (int i = 0; i < 2; ++i) {
            *((char*)pom+i+structSize) = '#';
        }
        for (int i = 0; i < 2; ++i) {
            *((char*)pom+i+size+structSize+2) = '#';
        }
        memory_manager.hashSum = hashSum();
        return (char *)pom + structSize + 2;
    }
    return NULL;
}
void* heap_calloc(size_t number, size_t size)
{
    size_t structSize = sizeof(struct memory_chunk_t);
    if(!size|| heap_validate() || !number)
        return NULL;

    if(!memory_manager.first_memory_chunk)
    {
        memory_manager.memory_start = custom_sbrk(size*number+4+structSize);
        if(memory_manager.memory_start == (void*)-1)
        {
            memory_manager.memory_start = NULL;
            return NULL;
        }
        struct memory_chunk_t *first = (struct memory_chunk_t *)memory_manager.memory_start;
        first->next = NULL;
        first->prev = NULL;
        first->free = 0;
        first->size = size * number;
        allsizemalloc += size * number + 4 + structSize;

        for (int i = 0; i < 2; ++i) {
            *((char*)first+i+structSize) = '#';
            *((char*)first+i+size*number+structSize+2) = '#';
        }
        for (unsigned long i = 0;i < number*size; ++i) {
            *((char*)first+i+structSize+2) = 0;
        }
        first->hash = getHash(first);

        memory_manager.first_memory_chunk = first;
        memory_manager.hashSum = hashSum();
        return (char *)first + structSize + 2;
    }
    else
    {
        struct memory_chunk_t *prevPom = NULL;
        struct memory_chunk_t *pom = memory_manager.first_memory_chunk;
        for(; pom ; prevPom = pom, pom=pom->next)
        {
            if((pom->size >= size*number + 4 && pom->free))
                break;
        }
        if(pom)
        {
            for (int i = 0; i < 2; ++i) {
                *((char*)pom+i+structSize) = '#';
            }
            for (unsigned long i = 0;i < number*size; ++i) {
                *((char*)pom+i+structSize+2) = 0;
            }
            for (int i = 0; i < 2; ++i) {
                *((char*)pom+i+size*number+structSize+2) = '#';
            }

            pom->free = 0;
            pom->size = size * number;
            allsizemalloc += size * number + 4;
            pom->hash = getHash(pom);
            memory_manager.hashSum = hashSum();
            return (char *)pom + structSize + 2;
        }


        pom = custom_sbrk(size*number+4+structSize);
        if(pom == (void*)-1)
        {
            pom = NULL;
            return NULL;
        }
        allsizemalloc += size * number + 4 + structSize;
        prevPom->next = pom;
        pom->next = NULL;
        pom->prev = prevPom;
        pom->size = size * number;
        pom->free = 0;
        pom->hash = getHash(pom);
        prevPom->hash = getHash(prevPom);
        for (int i = 0; i < 2; ++i) {
            *((char*)pom+i+structSize) = '#';
        }
        for (unsigned long i = 0;i < number*size; ++i) {
            *((char*)pom+i+structSize+2) = 0;
        }
        for (int i = 0; i < 2; ++i) {
            *((char*)pom+i+size*number+structSize+2) = '#';
        }
        memory_manager.hashSum = hashSum();
        return (char *)pom + structSize + 2;
    }
}
void* heap_realloc(void* memblock, size_t count)
{
    if(heap_validate())
        return NULL;
    if(!memblock && !count)
        return NULL;
    if(!memblock)
    {
        return heap_malloc(count);
    }
    else if(!count)
    {
        heap_free(memblock);
        return NULL;
    }
    size_t structSize = sizeof(struct memory_chunk_t);
    struct memory_chunk_t *pom = memory_manager.first_memory_chunk;
    for (;pom;pom=pom->next) {
        if((char*)pom + structSize + 2 == (char*)memblock)
            break;
    }
    if(!pom)
        return NULL;
    if(pom->size < count)
    {
        if(pom->next && pom->next->free)
        {
            size_t actualSize = 0;
            struct memory_chunk_t* pom3 = pom;
            for (int i = 0; i < 2; ++i, pom3=pom3->next) {
                if(pom3->next)
                    actualSize += (size_t)((char *)pom3->next - (char*)pom3) - structSize;
                else
                    actualSize += pom3->size;
            }
            if(actualSize + structSize >= count)
            {
                heap_free(memblock);
                return heap_malloc(count);
            }

            void *pom2 = heap_malloc(count);
            if(!pom2)
                return NULL;
            for (int i = 0; i < (int)pom->size; ++i) {
                *((char*)pom2 + i) = *((char*)memblock + i);
            }
            pom = memory_manager.first_memory_chunk;
            for (;pom;pom=pom->next) {
                pom->hash = getHash(pom);
            }
            heap_free(memblock);



//            pom = memory_manager.first_memory_chunk;
//            for (;pom;pom=pom->next) {
//                pom->hash = getHash(pom);
//            }

            return pom2;
        }
        if(!pom->next)
        {
            memory_manager.memory_start = custom_sbrk(count - pom->size );
            if(memory_manager.memory_start == (void*)-1)
            {
                pom = NULL;
                return NULL;
            }
            allsizemalloc += count - pom->size;
            pom->size = count;
            pom->hash = getHash(pom);
            for (int i = 0; i < 2; ++i) {
                *((char*)pom + structSize + i) = '#';
                *((char*)pom + structSize + i + pom->size + 2) = '#';
            }
            return memblock;
        }
        size_t prevSize = pom->size;
        heap_free(memblock);
        memblock = heap_malloc(count);
        if(!memblock)
            return NULL;

        for (int i = 0; i < (int)prevSize; ++i) {
            *((char*)memblock + i) = *((char*)pom + structSize + 2 + i);
        }
        return memblock;
    }
    else if(pom->size > count)
    {
        pom->size = count;

        for (int i = 0; i < 2; ++i) {
            *((char*)pom + structSize + i) = '#';
            *((char*)pom + structSize + i + pom->size + 2) = '#';
        }
        pom->hash = getHash(pom);
        return memblock;
    }
    else
        return memblock;


    return NULL;
}

int free_number(struct memory_chunk_t *chunk, int *size)
{
    int i;
    *size = 0;
    for (i = 0; chunk; ++*size, chunk = chunk->next) {
        if(chunk->free == 1)
            i++;
    }
    return i;
}

void heap_free(void* memblock)
{
    if(!memblock || heap_validate() || get_pointer_type(memblock) != pointer_valid)
        return;
    size_t structSize = sizeof(struct memory_chunk_t);
    struct memory_chunk_t *pom = memory_manager.first_memory_chunk;

    int nodeNumber;
    int freeNumber = free_number(pom,&nodeNumber);

    if(freeNumber == nodeNumber && nodeNumber == 1)
    {
        allsizefree += memory_manager.first_memory_chunk->size + structSize + 4;
        memory_manager.first_memory_chunk = custom_sbrk(-1*(intptr_t)(memory_manager.first_memory_chunk->size+structSize+4));
        memory_manager.first_memory_chunk = NULL;
        return;
    }
    for(int i=0;pom;pom=pom->next,i++)
    {
        if((char*)pom + structSize + 2 == (char *)memblock)
        {
            pom->free = 1;
            if(pom->next)
                pom->size = (size_t)((char *)pom->next - (char*)pom) - structSize;
            freeNumber++;
            break;
        }
    }

    pom = memory_manager.first_memory_chunk;
    int flag = 0;
    for (int i = 0; pom->next; ++i )
    {
        if(pom->free && pom->next->free)
        {
            pom->size += pom->next->size + structSize;

            if(pom->next->next)
                pom->next->next->prev = pom;
            i--;
            pom->next = pom->next->next;
            pom->hash = getHash(pom);
            if(pom->next)
                pom->next->hash = getHash(pom->next);
            nodeNumber--;
            freeNumber--;
            flag=1;
        }
        if(!flag)
        {
            pom = pom->next;
        }
        flag = 0;
    }

    if(freeNumber == nodeNumber == 1)
    {
        size_t totalSize = memory_manager.first_memory_chunk->size+structSize+4;

        memory_manager.first_memory_chunk = custom_sbrk(-(intptr_t)(memory_manager.first_memory_chunk->size+structSize+4));
        if(memory_manager.first_memory_chunk == (void*)-1)
        {
            memory_manager.first_memory_chunk = NULL;
            printf("\n\nnie powiodlo sie\n\n");
            return;
        }
        allsizefree += totalSize;
        printf("\n\n404 liczba zwolnienia allsizemalloc: %d       allsizefree: %d\n\n", (int)allsizemalloc, (int)allsizefree);
        memory_manager.hashSum = 0;
        allsizefree = 0;
        allsizemalloc = 0;
        memory_manager.first_memory_chunk = NULL;
        return;
    }
    pom = memory_manager.first_memory_chunk;
    int i;
    for (i = 0; i<nodeNumber-1; pom=pom->next, i++);
    if(pom->free && nodeNumber != 1)
    {
        pom->prev->next = NULL;
        pom = custom_sbrk(-1*(intptr_t)(pom->size+structSize+4));
        allsizefree += pom->size + structSize + 4;
        printf("\n\n417 liczba zwolnienia allsizemalloc: %d       allsizefree: %d\n\n", (int)allsizemalloc, (int)allsizefree);
        pom = NULL;
    }

    pom = memory_manager.first_memory_chunk;
    for (; pom;pom=pom->next) {
        pom->hash = getHash(pom);
    }
}
enum pointer_type_t get_pointer_type(const void* const pointer)
{
    size_t structSize = sizeof(struct memory_chunk_t);
    if(pointer == NULL)
        return pointer_null;
    else if(heap_validate())
        return pointer_heap_corrupted;
    struct memory_chunk_t *pom = memory_manager.first_memory_chunk;
    for(int i=0;pom;pom=pom->next,i++)
    {
        if((char*)pom <= (char*)pointer && (char*)pom + structSize > (char*)pointer)
        {
            if(pom->free)
                return pointer_unallocated;
            return pointer_control_block;
        }
        if((char*)pointer == (char*)pom + structSize ||  (char*)pointer == (char*)pom + structSize + 1 || (char*)pointer == (char*)pom + structSize + pom->size + 2|| (char*)pointer == (char*)pom + structSize + pom->size + 3)
        {
            if(pom->free)
                return pointer_unallocated;
            return pointer_inside_fences;
        }
        if((char*)pom + structSize + 2 == (char *)pointer)
        {
            if(pom->free == 0)
                return pointer_valid;
            else
                return pointer_unallocated;
        }


        if((char*)(pom) + structSize + 2 < (char *)pointer && (char*)(pom) + structSize + 2 + pom->size >= (char*)pointer)
        {
            if(pom->free)
                return pointer_unallocated;
            return pointer_inside_data_block;
        }
    }
    return pointer_unallocated;

}

int hashSum(void)
{
    struct memory_chunk_t *pom = memory_manager.first_memory_chunk;
    int sum = 0;
    for (; pom; pom=pom->next) {
        sum += getHash(pom);
    }
    return sum;
}
int getHash(struct memory_chunk_t *pom)
{
    int hashCode = 0;
    for (int i = 0; i < 28; ++i) {
        hashCode += (int)*((char*)pom + i);
    }
    return hashCode;
}

int heap_validate(void)
{
    if(!memory_manager.memory_start)
        return 2;
    struct memory_chunk_t *pom = memory_manager.first_memory_chunk;
    for (; pom; pom=pom->next) {
        if(pom->hash != getHash(pom))
            return 3;
    }
    pom = memory_manager.first_memory_chunk;
    size_t structSize = sizeof(struct memory_chunk_t);
    for (; pom; pom=pom->next) {
        if(pom && pom->free)
            continue;
        for (int i = 0; i < 2; ++i) {
            if(*((char*)pom + structSize + i) != '#' && !pom->free)
                return 1;
            if(*((char*)pom + structSize + pom->size + i + 2) != '#' && !pom->free)
                return 1;
        }
    }
    return 0;
}
size_t heap_get_largest_used_block_size(void)
{
    if(heap_validate())
        return 0;
    size_t largest = 0;
    struct memory_chunk_t* pom = memory_manager.first_memory_chunk;
    for (; pom; pom = pom->next) {
        if(!pom->free && pom->size>largest)
            largest = pom->size;
    }
    return largest;
}









