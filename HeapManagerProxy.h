#pragma once
#include "HeapManager.h"
#include "FixedSizeAllocator.h"

namespace HeapManagerProxy {

	HeapManager* CreateHeapManager(void* pHeapMemory, size_t sizeHeap, unsigned int numDescriptors);

	void* alloc(HeapManager* pHeapManager, const size_t sizeAlloc);
	
	void* alloc(HeapManager* pHeapManager, const size_t sizeAlloc, const unsigned int alignment);
	
	void Collect(HeapManager* pHeapManager);

	bool Contains(HeapManager* pHeapManager, void* pPtr);

	bool IsAllocated(HeapManager* pHeapManager, void* pPtr);

	bool free(HeapManager* pHeapManager, void* pPtr);

	void Destroy(HeapManager* pHeapManager);

	void ShowFreeBlocks(HeapManager* pHeapManager);

	void ShowOutstandingAllocations(HeapManager* pHeapManager);

	FixedSizeAllocator* CreateFixedSizeAllocator(HeapManager* pHeapManager, unsigned long fixedSize, unsigned int blockNum);

}

