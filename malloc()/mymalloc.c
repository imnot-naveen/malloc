#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mymalloc.h"

// defining memory in bytes
#define memory_size 25000

// array to allocate storage
char memory[memory_size];

// structure to store data for used and free memory blocks and representation of a memory block
typedef struct mem_block
{
    size_t length;
    struct mem_block *next;
} mem_block;

// calculating the size of a memory block
const size_t block_size = sizeof(mem_block);

/*
    for the ease of use, we will keep two signly linked lists to keep track of the free and used blocks of memory
*/

mem_block *freelist = NULL;
mem_block *usedlist = NULL;

// declaration of the functions used
mem_block *findfreeblock(size_t size);
void addBlock(mem_block **start, mem_block **block);
void delBlock(mem_block **start, mem_block *block);

/*
    mymalloc() function is a much more simpler implementation of the malloc() fuction that comes with the standard C library. It will allocate
    memory block of size that is required by the user. If it cannot allocate a CONTIGUOUS memory block of size that is required by the user,
    it will return NULL. If a mempry block is successfully allocated, it will return a pointer to the first address of the memory block
*/

void *mymalloc(size_t size)
{
    // block to be allocated is larger than the memory itself
    if (size > memory_size - block_size)
    {
        return NULL;
    }

    // allocating memory for the first time
    // in this case both free and used lists are empty
    if (freelist == NULL && usedlist == NULL)
    {
        mem_block *ptr = (mem_block *)&memory[0];
        ptr->length = memory_size - block_size;
        addBlock(&freelist, &ptr);
    }

    // finding a suitable memory block in the free list
    mem_block *ptr = findfreeblock(size);

    // if a block isn't found, return NULL
    if (ptr == NULL)
    {
        return NULL;
    }

    // remove the  found freeblock from the free list
    size_t remaining_size = ptr->length - size;
    delBlock(&freelist, ptr);

    // add the newly allocated blok to the used list
    mem_block *used_block = (mem_block *)&memory[(size_t)((char *)ptr - memory)];
    used_block->length = size;
    addBlock(&usedlist, &used_block);

    /*
        once we find a free block and allocate the required block, theres a remaining  memory block from that initial free block. We will add that portion of the free
        block only if it is a big enough to hold the memory block structure, else it will be considered as wasted space and will not be considered for future allocations
    */

    if (remaining_size > block_size)
    {
        mem_block *free_block = (mem_block *)&memory[(size_t)((char *)ptr - memory) + size + block_size];
        free_block->length = remaining_size - block_size;
        addBlock(&freelist, &free_block);
    }

    // since we have successfully allocated a memory block we can return the starting address to that allocated space
    return (void *)&memory[(size_t)((char *)ptr - memory) + block_size];
}

/*
    myfree() is a simpler implementation of free() that comes with the standard C library. It frees the memory that is allocated by malloc. Functionality can be explained as
    follows. It takes a pointer to a memory block as argument. Then it traverses through the used list to find the relevant memory block and remove it from the used list and add
    it to the free list.
*/

void myfree(void *ptr)
{
    // if the argument pointer is NULL, function does nothing
    if (ptr == NULL)
    {
        return;
    }

    // identiying the real pointer
    mem_block *realptr = (mem_block *)&memory[(size_t)((char *)ptr - memory) - block_size];
    mem_block *temp = usedlist;

    // flag to make sure if the relevant block is found
    bool flag = false;

    if (realptr == usedlist)
    {
        flag = true;
    }

    // find the used block to be de allocated
    while (temp->next != usedlist && flag)
    {
        if (temp == realptr)
        {
            flag = true;
        }
        temp = temp->next;
    }
    if (temp == realptr)
    {
        flag = true;
    }

    // if the given pointer is not available in the used list, function does nothing
    if (!flag)
    {
        return;
    }

    // remove the given block from the used list
    delBlock(&usedlist, realptr);

    // add the given block to the free list
    addBlock(&freelist, &realptr);
}

// function that is used to traverse the freelist and find a suitable free block for allocation. Returns NULL if a suitable block is not found.
mem_block *findfreeblock(size_t size)
{
    mem_block *ptr = freelist;
    if (size <= ptr->length)
        return ptr;
    while (ptr->next != freelist)
    {
        if (size <= ptr->length)
            return ptr;
        ptr = ptr->next;
    }
    return NULL;
}

// function that is used to add a memory block to a list. Blocks will be sorted in the ascending order of their addresses.
void addBlock(mem_block **start, mem_block **block)
{
    mem_block *ptr = *start;
    if (*start == NULL)
    {
        *start = *block;
        (*block)->next = *block;
        return;
    }
    if ((*start)->next == *start && (long long)*block < (long long)*start)
    {
        (*block)->next = *start;
        while (ptr->next != *start)
            ptr = ptr->next;
        ptr->next = *block;
        *start = *block;
        return;
    }
    ptr = ptr->next;
    mem_block *preptr = *start;
    while (ptr != *start)
    {
        if ((long long)*block < (long long)ptr)
        {
            break;
        }
        preptr = ptr;
        ptr = ptr->next;
    }
    (*block)->next = ptr;
    preptr->next = (*block);
}

// function to delete a block from a given list
void delBlock(mem_block **start, mem_block *block)
{
    mem_block *ptr = *start;
    mem_block *preptr = *start;
    if ((*start)->next == *start)
    {
        *start = NULL;
        return;
    }
    if (block == *start)
    {
        while (ptr->next != *start)
            ptr = ptr->next;
        ptr->next = (*start)->next;
        *start = (*start)->next;
        return;
    }
    while (ptr != block)
    {
        preptr = ptr;
        ptr = ptr->next;
    }
    preptr->next = ptr->next;
}

int main()
{
    int *arr = mymalloc(sizeof(int) * 8);
    for (int i = 0; i < 8; i++)
    {
        arr[i] = i + 1;
    }
    for (int i = 0; i < 8; i++)
    {
        printf("%d ", arr[i]);
    }
}