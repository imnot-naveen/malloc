/*
    SCS1214 Operating Systems I
    21002241 - R.B. Wimalasena
    My implementation of malloc() and free(). MyMalloc() and MyFree() have similar
    functionalities to malloc() and free() but they are not identical. Behaviour
    of the following functions are undefined for some edge cases just like in
    malloc() and free().

                    *** ISSUES WITH THIS IMPLEMENTATION ***
    * External fragmentation
    * Fragmented adjacent free blocks
    * Memory allocation for metadata is done using the memory array itself
    * "Wasted memory" can occur because of the free block linked list's metadata
    * First-fit may not be the optimal solution for each and every problem

                        *** HOW TO FIX THE ISSUES ***
    * Implement the entire memory management library including free() and malloc()
      with merge free blocks implementation to avoid external fragmentation
    * use other algorithms to search for free memory(eg:- best-fit, worst-fit, etc.)

                          *** OTHER INFORMATION ***
    * Maximum allocatable size 24984 Bytes
    * metadata block size 16 Bytes
*/

#include <stdio.h>

// header guard for conditional compilation
#ifndef MYMALLOC_H_
#define MYMALLOC_H_

void *MyMalloc(size_t size);
void MyFree(void *ptr);

#endif