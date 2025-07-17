#ifndef alloc_H
#define alloc_H

#include <elios.h>

Elios_Public void *mem_alloc(int32 size);
Elios_Public void  mem_dealloc(void *ptr);
Elios_Public void *mem_realloc(void *ptr, int32 size);

#endif // alloc_H