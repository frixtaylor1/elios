#include <allocators/pool_alloc.h>
#include <allocators/alloc.h>
#include <assert.h>

Elios_Public void pool_init(PoolAllocator *poolAllocator, int32 maxItems, int32 itemSize) {
	poolAllocator->arena    = mem_alloc(maxItems * itemSize);
	poolAllocator->nbItems  = 0;
	poolAllocator->itemSize = itemSize;
	poolAllocator->capacity = maxItems * itemSize;
}

Elios_Public void *pool_mem_alloc(PoolAllocator *poolAllocator) {
	int32 currentSpentCap = poolAllocator->nbItems * poolAllocator->itemSize;
	assert(currentSpentCap < poolAllocator->capacity && "Max capacity of pool allocator reached.");
	
	void *ptr = &poolAllocator->arena[currentSpentCap];
	poolAllocator->nbItems++;
	return ptr;
}

Elios_Public void pool_dealloc(PoolAllocator *poolAllocator) {
	mem_dealloc(poolAllocator->arena);
}