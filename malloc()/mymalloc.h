/*
        @imnot_naveen

            This program is a much simpler version of malloc() and free() which is included with the standard C library.
            It allocates memory from a array of 250000 bytes and frees them.
            Program does not intend to replace malloc() and free() under any circumstances.
*/
#include <stdio.h>
// header guard for conditional compilation
#ifndef MYMALLOC_H_
#define MYMALLOC_H_

void *mymalloc(size_t size);
void myfree(void *ptr);

#endif