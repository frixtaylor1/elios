#ifndef alloc_H
#define alloc_H

#include <elios.h>

void *mem_alloc(int32 size);
void  mem_dealloc(void *ptr);
void *mem_realloc(void *ptr, int32 size);

#endif // alloc_H