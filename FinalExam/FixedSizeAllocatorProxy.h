#pragma once
#include "FixedSizeAllocator.h"


namespace FixedSizeAllocatorProxy {
	
	FixedSizeAllocator* CreateFixedSizeAllocator(void* baseAddr, int fixedSize, int blockNum);

	void* alloc(FixedSizeAllocator* pFixedSizeAllocator, size_t sizeAlloc);

	bool free(FixedSizeAllocator* pFixedSizeAllocator, void* ptr);
	
	bool Contains(void* ptr);

	void Destroy(FixedSizeAllocator* pFixedSizeAllocator);

}