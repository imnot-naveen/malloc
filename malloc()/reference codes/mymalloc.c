/*
    SCS1214 Operating Systems I
    21002241 - R.B. Wimalasena
*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mymalloc.h"
// Memory size in bytes
#define MEM_SIZE 25000

// Array to allocate space for dynamic data storage
char memory[MEM_SIZE];

// Structure that is used to store overhead data for both free blocks and used blocks
typedef struct over_head
{
    size_t length;
    struct over_head *next;
} ov_head;

// Overhead size in bytes.
// Overhead of a free block (Minimum allocatable size) and overhead of an used block
const size_t OH_SIZE = sizeof(ov_head);

// Starting pointers of the free list and used list
/*
    Singly cicular linked list is used because if the free block search algorithm has
    to changed in the future(to best-fit, worst-fit, etc.), it can be easily implemented
*/
ov_head *free_list = NULL;
ov_head *used_list = NULL;

ov_head *findFreeBlock(size_t size);
void addBlock(ov_head **start, ov_head **block);
void delBlock(ov_head **start, ov_head *block);
void displayList(ov_head *start);

/*
    MyMalloc() will allocate a memory block which is equal to the size of the argument
    given by the user. Upon failure (there is not enough contiguous memory to fullfill
    the allocation) MyMalloc() will return a NULL pointer. Upon success it will return
    the first memory address of the allocated memory block.
    MyMalloc() keeps track of a used memory blocks list and free memory blocks list,
    using the bytes of the used and free memory block respectively. MyMalloc() traverse
    through the free list to find a free block which is larger than or equal to the
    requested size. Then the free block is divided into a used block and if the remaining
    space is sufficient, into a free block. Old free block is removed from the free
    list and the new free block is added to the list. The allocated block is added to the
    used list and the starting address of the block is returned.
    ** If MyMalloc() fails to find a suitable memory block, it returns NULL.
    ** If MyMalloc(0) is called small memory block is allocated and should not be used
       for any calculations.
    ** MyMalloc() returns memory blocks un-initialized (containing garbage values).
*/
void *MyMalloc(size_t size)
{
    // Allocating block is larger than the memory itself
    if (size > MEM_SIZE - OH_SIZE)
        return NULL;

    // Allocating memory for the first time
    if (free_list == NULL && used_list == NULL)
    {
        ov_head *ptr = (ov_head *)&memory[0];
        ptr->length = MEM_SIZE - OH_SIZE;
        addBlock(&free_list, &ptr);
        // printf("Allocating for the first time %p->%d | f->%p\n",ptr,ptr->length,free_list);
    }

    // Find a suitable memory block in the free list
    ov_head *ptr = findFreeBlock(size);

    // If a suitable free block is not found, return NULL
    if (ptr == NULL)
    {
        return NULL;
    }
    // printf("Found block %p->%d\n",ptr,ptr->length);

    // Remove the relevent free block from the free list
    size_t rem_size = ptr->length - size;
    delBlock(&free_list, ptr);

    // Add the newly allocated block to the used list
    ov_head *u_block = (ov_head *)&memory[(size_t)((char *)ptr - memory)];
    u_block->length = size;
    addBlock(&used_list, &u_block);
    // printf("Allocated block %p->%d\n",u_block,u_block->length);

    /*
       Add the remaining portion of the free block to the free list only if it is large
       enough to hold the overhead structure(in order it to be in the free list).
       If it is not large enough it won't be used for future allocations and will
       be considered as "wasted space".
    */
    if (rem_size > OH_SIZE)
    {
        ov_head *f_block = (ov_head *)&memory[(size_t)((char *)ptr - memory) + size + OH_SIZE];
        f_block->length = rem_size - OH_SIZE;
        addBlock(&free_list, &f_block);
        // printf("Free block %p->%d\n",f_block,f_block->length);
    }

    // Return the staring address of the allocated memory space
    // printf("Return value %p\n",&memory[(size_t)((char*)ptr-memory) + OH_SIZE]);
    return (void *)&memory[(size_t)((char *)ptr - memory) + OH_SIZE];
}

/*
    MyFree() frees up a memory block allocated by MyMalloc(). MyFree() traverses
    used blocks list to find the relevant memory block. This block is removed
    from the used blocks list and added to the free blocks list. When memory is
    being freed some adjacent memory blocks may be free at once, but will be
    identified as small free blocks. This is resolved by the memory management
    library, the free() implementation it self does not resolve this issue. free()
    only adds the free block into the free list. Merging and the defragmentation
    done by the memory menagement library that comes with malloc and free
    (eg:- allocate, deallocate, reallocate, compact, fragmentation_ratio, etc.)
    ** Behaviour is undefined if the argument isn't a pointer returned by MyMalloc().
    ** If the argument is NULL, then MyFree() does nothing.
    ** Behaviour is undefined if the area to be de-allocated is already de-allocated.
*/
void MyFree(void *ptr)
{
    // If the argument pointer is NULL, the function does nothing.
    if (ptr == NULL)
        return;

    // Identifying the real pointer with the overhead
    // printf("Pointer given as the argument %p\n",ptr);
    ov_head *real_ptr = (ov_head *)&memory[(size_t)((char *)ptr - memory) - OH_SIZE];
    // printf("Real pointer %p\n",real_ptr);
    ov_head *temp = used_list;

    bool flag = false;

    // If the pointer to be de-allocated is the first block os the used list.
    if (real_ptr == used_list)
    {
        flag = true;
    }

    // Find the used block to be de-allocated
    while (temp->next != used_list && !flag)
    {
        if (temp == real_ptr)
        {
            flag = true;
        }
        // printf("Used block %p\n",temp);
        temp = temp->next;
    }
    if (temp == real_ptr)
        flag = true;

    // If the given pointer is not available in the used list, function does nothing
    if (!flag)
    {
        // printf("Given pointer %p is invalid\n",ptr);
        return;
    }

    // Remove the given block from the used list
    // printf("before removing from used list Used list->%p | Real Pointer->%p\n",used_list,real_ptr);
    delBlock(&used_list, real_ptr);
    // printf("After removing from used list Used list->%p | Real Pointer->%p\n",used_list,real_ptr);

    // Add the given block to the free list
    // printf("before adding to the free list Free list->%p | Real Pointer->%p\n",free_list,real_ptr);
    addBlock(&free_list, &real_ptr);
    // printf("after adding to the free list Free list->%p | Real Pointer->%p\n",free_list,real_ptr);
}

// Used to traverse the free list and find a suitable free block for the given size
// Returns NULL if a suitable block is not found
ov_head *findFreeBlock(size_t size)
{
    ov_head *ptr = free_list;
    if (size <= ptr->length)
        return ptr;
    while (ptr->next != free_list)
    {
        if (size <= ptr->length)
            return ptr;
        ptr = ptr->next;
    }
    return NULL;
}

/* Add a block to a given linked list, blocks will be sorted based on their addresses
   in ascending order */
void addBlock(ov_head **start, ov_head **block)
{
    ov_head *ptr = *start;
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
    ov_head *preptr = *start;
    while (ptr != *start)
    {
        // printf("block->%lld ptr->%lld\n",(long long)*block,(long long)ptr);
        if ((long long)*block < (long long)ptr)
        {
            // printf("block %p & ptr %p\n",*block,*ptr);
            break;
        }
        preptr = ptr;
        ptr = ptr->next;
    }
    (*block)->next = ptr;
    preptr->next = (*block);
    displayList(*start);
}

// Delete a given block from a given linked list
void delBlock(ov_head **start, ov_head *block)
{
    ov_head *ptr = *start;
    ov_head *preptr = *start;
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
    displayList(*start);
}

// Display the given list free/used
void displayList(ov_head *start)
{
    ov_head *ptr = start;
    if (start == NULL)
    {
        // printf("List is empty\n");
        return;
    }
    if (start->next == start)
    {
        // printf("List Item %p\n",start);
        return;
    }
    while (ptr->next != start)
    {
        // printf("List Item %p\n",ptr);
        ptr = ptr->next;
    }
    // printf("List Item %p\n",ptr);
}

int main()
{
    int *arr;
    arr = MyMalloc(sizeof(int) * 0);
    for (int i = 0; i < 8; i++)
    {
        arr[i] = i;
    }
    for (int i = 0; i < 8; i++)
    {
        printf("%d ", arr[i]);
    }
}