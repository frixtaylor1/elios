#ifndef pool_alloc_H
#define pool_alloc_H

#include <elios.h>

typedef struct PoolAllocator {
	char  *arena;
	int32 nbItems;
	int32 itemSize;
	int32 capacity;
} PoolAllocator;

Elios_Public void  pool_init(PoolAllocator *poolAllocator, int32 maxItems, int32 itemSize);
Elios_Public void *pool_mem_alloc(PoolAllocator *poolAllocator);
Elios_Public void  pool_dealloc(PoolAllocator *poolAllocator);

#endif // pool_alloc_H